/**
 * @file ConnectionManager.cpp
 * @author Edward Palmer
 * @date 2025-08-11
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ConnectionManager.hpp"
#include "logger/Logger.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <optional>
#include <poll.h>
#include <signal.h>
#include <stdexcept>
#include <thread>
#include <unistd.h>


ConnectionManager::~ConnectionManager()
{
    stop();
    wait(); /* Ensure logger has shutdown */
}


void ConnectionManager::start()
{
    if (_active)
    {
        return;
    }

    _active = true;

    /* Startup logger */
    Logger::instance().start();
    Logger::instance().info("Starting-up server...");

    /* Create thread for processing received messages */
    _handleMessageLoopThread = std::thread(&ConnectionManager::handleMessageLoop, this);

    /* Create thread for cleaning up inactive connections */
    _cleanupInactiveSessionsThread = std::thread(&ConnectionManager::cleanupInactiveSessionsLoop, this);

    /* Any custom startup logic in subclass */
    onStartup();

    Logger::instance().info("Startup completed.");
}


void ConnectionManager::wait()
{
    if (_handleMessageLoopThread.joinable())
        _handleMessageLoopThread.join();
    if (_cleanupInactiveSessionsThread.joinable())
        _cleanupInactiveSessionsThread.join();

    onWait(); /* Wait hook for subclasses */

    Logger::instance().wait();
}


void ConnectionManager::stop()
{
    if (!_active)
    {
        return;
    }

    Logger::instance().info("Shutting-down server...");

    _active = false;                  /* Listening, cleanup loops poll regularly => shutdown on _active=false */
    _incomingMsgQueueCV.notify_all(); /* Trigger handleMessageLoop shutdown */

    /* Cleanup all sessions now marked as inactive */
    markAllSessionsAsInactive();
    cleanupInactiveSessions();

    onShutdown(); /* Shutdown hook (note: prior to logger shutdown) */

    /* Shutdown logger */
    Logger::instance().info("Shutdown completed.");
    Logger::instance().stop();
}


bool ConnectionManager::connectToServer(Port serverPort)
{
    /* Address of server we would like to connect to */
    SocketFD serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == (-1))
    {
        Logger::instance().log("Failed to create socket for port " + std::to_string(serverPort), Logger::Error);
        return false;
    }

    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;                     /* IPV4 */
    serverAddress.sin_port = htons(serverPort);             /* Port */
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); /* Connect to localhost */

    if (connect(serverSocket, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)) == (-1))
    {
        Logger::instance().log("Failed to establish connection to server (socket: " + std::to_string(serverSocket) + ")", Logger::Error);
        return false;
    }

    /* Register */
    _portSocketMappings.update(serverPort, serverSocket);

    Logger::instance().log("Established connection to server (socket: " + std::to_string(serverSocket) + ")");
    addClientSession(serverSocket);
    return true;
}


void ConnectionManager::handleMessageLoop()
{
    Logger::instance().info("Starting handleMessageLoop");

    while (true) /* Run for server lifetime */
    {
        std::unique_lock lock(_incomingMsgQueueMutex); /* Wait until we have messages in the queue */
        _incomingMsgQueueCV.wait(lock, [this]()
        {
            return (!_active || !_incomingMsgQueue.empty());
        });

        if (!_active)
        {
            break;
        }

        auto clientMessage = _incomingMsgQueue.front();
        _incomingMsgQueue.pop();

        handleMessage(std::move(clientMessage.first), clientMessage.second);
    }

    Logger::instance().info("Shutting-down handleMessageLoop");
}


void ConnectionManager::addClientSession(SocketFD clientSocket)
{
    if (clientSocket == (-1))
    {
        Logger::instance().log("Invalid client socket (" + std::to_string(clientSocket) + ")", Logger::Error);
        return;
    }

    auto session = std::make_unique<ClientSession>(clientSocket);
    session->active = true;

    session->connectionThread = std::thread(&ConnectionManager::connectionLoop, this, std::ref(*session));
    session->senderThread = std::thread(&ConnectionManager::senderLoop, this, std::ref(*session));

    {
        std::unique_lock lock(_clientSessionMutex);
        _clientSessionMap[clientSocket] = std::move(session);
    }
}


void ConnectionManager::markSessionAsInactive(ClientSession &session)
{
    if (session.active)
    {
        session.active = false;
        session.outgoingCV.notify_all(); /* Notify all loops which will shutdown */
    }
}


void ConnectionManager::markAllSessionsAsInactive()
{
    std::unique_lock lock(_clientSessionMutex);
    for (auto iter = _clientSessionMap.begin(); iter != _clientSessionMap.end(); ++iter)
    {
        markSessionAsInactive(std::ref(*iter->second));
    }
}


void ConnectionManager::cleanupInactiveSessions()
{
    std::unique_lock lock(_clientSessionMutex);

    for (auto iter = _clientSessionMap.begin(); iter != _clientSessionMap.end();)
    {
        auto &session = iter->second;

        if (!session->active)
        {
            Logger::instance().info("Cleaning-up session (socket: " + std::to_string(session->clientSocket) + ")");
            if (session->connectionThread.joinable() && std::this_thread::get_id() != session->connectionThread.get_id())
                session->connectionThread.join();
            if (session->senderThread.joinable() && std::this_thread::get_id() != session->senderThread.get_id())
                session->senderThread.join();

            closeSocket(session->clientSocket);

            _portSocketMappings.erase(session->clientSocket);

            iter = _clientSessionMap.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}


void ConnectionManager::cleanupInactiveSessionsLoop()
{
    Logger::instance().info("Starting cleanup inactive sessions loop");

    while (_active)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        cleanupInactiveSessions();
    }

    Logger::instance().info("Shutting-down cleanup inactive sessions loop");
}


// TODO: - extend with 2 threads for handleFixMsg() -> need to both be able to routed corrections/cancellations
// TODO: - netadmin should be able to send shutdown

void ConnectionManager::connectionLoop(ClientSession &session)
{
    Logger::instance().info("Starting connection loop (socket: " + std::to_string(session.clientSocket) + ")");

    char messageBuffer[2048];

    struct pollfd fds;
    fds.fd = session.clientSocket;
    fds.events = POLLIN;

    while (session.active) /* Run for session lifetime */
    {
        int pollResult = poll(&fds, 1, 1000); /* polls once per second */

        if (pollResult == 0) /* No new data */
        {
            continue;
        }
        else if (pollResult == (-1))
        {
            Logger::instance().error("A polling error occurred (socket: " + std::to_string(session.clientSocket) + ")");
            continue;
        }

        /* Wipe buffer */
        memset(messageBuffer, 0, 2048);

        long nBytesRead = recv(session.clientSocket, messageBuffer, 2047, 0);

        if (nBytesRead == 0)
        {
            /* Orderly shutdown: -> need to send signal to senderThread and erase session from map */
            Logger::instance().info("Client connection has closed (socket: " + std::to_string(session.clientSocket) + ")");
            session.active = false;          /* Mark as inactive to initiate shutdown of sender loop */
            session.outgoingCV.notify_all(); /* Notify all loops */
            break;
        }
        else if (nBytesRead == (-1))
        {
            Logger::instance().error("An error occurred in recv() (socket: " + std::to_string(session.clientSocket) + ")");
            /* Continue for now */
        }
        else if (nBytesRead > 0)
        {
            auto clientMessage = std::pair(std::string(messageBuffer, nBytesRead), session.clientSocket);

            {
                std::unique_lock incomingMsgQueueLock(_incomingMsgQueueMutex);
                _incomingMsgQueue.push(std::move(clientMessage));
            } /* Unlock */

            _incomingMsgQueueCV.notify_one(); /* Notify the message queue loop to handle the received message */
        }
    }

    Logger::instance().info("Shutting-down connection loop (socket: " + std::to_string(session.clientSocket) + ")");
}


void ConnectionManager::senderLoop(ClientSession &session)
{
    Logger::instance().info("Starting sender loop (socket: " + std::to_string(session.clientSocket) + ")");

    while (session.active)
    {
        std::unique_lock lock(session.outgoingMutex);
        session.outgoingCV.wait(lock, [this, &session]()
        {
            return (!session.active || !session.outgoingMsgQueue.empty()); /* No longer active or messages to send */
        });

        if (!session.active)
        {
            break;
        }

        auto message = session.outgoingMsgQueue.front();

        long nBytesSent = send(session.clientSocket, message.c_str(), message.size(), 0);

        if (nBytesSent == (-1) || nBytesSent < static_cast<long>(message.size()))
        {
            Logger::instance().log("Failed to send message (destination: " + std::to_string(session.clientSocket) + "): " + message, Logger::Error);
        }
        else /* Sent successfully! */
        {
            session.outgoingMsgQueue.pop();
        }
    }

    Logger::instance().info("Shutting-down sender loop (socket: " + std::to_string(session.clientSocket) + ")");
}


void ConnectionManager::sendMessage(Message message, SocketFD clientSocket)
{
    std::shared_lock lock(_clientSessionMutex);
    auto iter = _clientSessionMap.find(clientSocket);
    if (iter == _clientSessionMap.end())
    {
        Logger::instance().log("No registered client socket " + std::to_string(clientSocket), Logger::Error);
        return;
    }

    if (!iter->second->active)
    {
        Logger::instance().log("Client session is inactive (socket: " + std::to_string(clientSocket) + ")", Logger::Error);
        return;
    }

    {
        std::lock_guard queueLock(iter->second->outgoingMutex); /* Acquire lock and push onto outgoing queue */
        iter->second->outgoingMsgQueue.push(std::move(message));
    }

    iter->second->outgoingCV.notify_one();
}


void ConnectionManager::closeSocket(SocketFD socket)
{
    if (close(socket) == (-1))
    {
        Logger::instance().error("Failed to close socket: " + std::to_string(socket));
    }
    else
    {
        Logger::instance().info("Closed socket: " + std::to_string(socket));
    }
}


void ConnectionManager::PortSocketMappings::clear()
{
    std::unique_lock lock(_portSocketMutex);
    _portToSocket.clear();
    _socketToPort.clear();
}


void ConnectionManager::PortSocketMappings::update(Port port, SocketFD socket)
{
    std::unique_lock lock(_portSocketMutex);
    _portToSocket[port] = socket;
    _socketToPort[socket] = port;
}


ConnectionManager::Port ConnectionManager::PortSocketMappings::getPort(SocketFD socket) const
{
    std::shared_lock lock(_portSocketMutex);

    auto iter = _socketToPort.find(socket);

    return (iter != _socketToPort.end()) ? iter->second : 0;
}


ConnectionManager::SocketFD ConnectionManager::PortSocketMappings::getSocket(Port port) const
{
    std::shared_lock lock(_portSocketMutex);

    auto iter = _portToSocket.find(port);

    return (iter != _portToSocket.end()) ? iter->second : (-1);
}


std::vector<ConnectionManager::SocketFD> ConnectionManager::PortSocketMappings::getSockets() const
{
    std::shared_lock lock(_portSocketMutex);

    std::vector<SocketFD> sockets;
    sockets.reserve(_socketToPort.size());

    for (auto iter = _socketToPort.begin(); iter != _socketToPort.end(); ++iter)
    {
        sockets.push_back(iter->first);
    }

    return sockets;
}


void ConnectionManager::PortSocketMappings::erase(SocketFD socket)
{
    Port port = getPort(socket);

    std::unique_lock lock(_portSocketMutex); /* Careful with lock since getPort has lock */
    _portToSocket.erase(port);
    _socketToPort.erase(socket);
}