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

    /* Create thread for processing received messages */
    _handleMessageLoopThread = std::thread(&ConnectionManager::handleMessageLoop, this);

    /* Any custom startup logic in subclass */
    onStartup();
}


void ConnectionManager::wait()
{
    if (_handleMessageLoopThread.joinable())
        _handleMessageLoopThread.join();

    onWait(); /* Wait hook for subclasses */

    Logger::instance().wait();
}


void ConnectionManager::stop()
{
    if (!_active)
    {
        return;
    }

    _active = false;

    Logger::instance().info("Shutting-down server...");

    _incomingMsgQueueCV.notify_all(); /* Trigger handleMessageLoop shutdown */
    /* Listening loop polls once per second => shutdown okay */

    /* Shutdown all client sessions */
    {
        std::unique_lock lock(_clientSessionMutex);
        for (auto &[socket, session] : _clientSessionMap)
        {
            closeClientSession(std::ref(*session));
        }

        _clientSessionMap.clear();
    }

    /* Wipe map of ports to sockets */
    _portSocketMappings.clear();

    onShutdown(); /* Shutdown hook (note: prior to logger shutdown) */

    /* Shutdown logger */
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
        Logger::instance().log("Failed to establish connection to server on socket " + std::to_string(serverSocket), Logger::Error);
        return false;
    }

    /* Register */
    _portSocketMappings.update(serverPort, serverSocket);

    Logger::instance().log("Established connection to server on socket " + std::to_string(serverSocket));
    addClientSession(serverSocket);
    return true;
}


void ConnectionManager::handleMessageLoop()
{
    Logger::instance().log("Starting handleMessageLoop...");

    while (_active) /* Run for server lifetime */
    {
        std::unique_lock lock(_incomingMsgQueueMutex); /* Wait until we have messages in the queue */
        _incomingMsgQueueCV.wait(lock, [this]()
        {
            return (!_active || !_incomingMsgQueue.empty());
        });

        if (!_active)
        {
            return;
        }

        auto clientMessage = _incomingMsgQueue.front();
        _incomingMsgQueue.pop();

        handleMessage(std::move(clientMessage.first), clientMessage.second);
    }
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


void ConnectionManager::removeClientSession(SocketFD clientSocket)
{
    std::unique_lock lock(_clientSessionMutex);
    auto iter = _clientSessionMap.find(clientSocket);
    if (iter == _clientSessionMap.end())
    {
        Logger::instance().log("No client socket found in map (" + std::to_string(clientSocket) + ")");
        return;
    }

    iter->second->active = false;          /* Mark as inactive to initiate shutdown of connection, sender loops */
    iter->second->outgoingCV.notify_all(); /* Notify all loops */

    closeClientSession(std::ref(*iter->second));
    _clientSessionMap.erase(iter); /* Wipe from map */

    _portSocketMappings.erase(clientSocket);
}


void ConnectionManager::closeClientSession(ClientSession &session)
{
    if (!session.active)
        return;

    session.active = false;          /* Mark as inactive to initiate shutdown of connection, sender loops */
    session.outgoingCV.notify_all(); /* Notify all loops */

    if (session.connectionThread.joinable()) /* Join threads */
        session.connectionThread.join();
    if (session.senderThread.joinable())
        session.senderThread.join();

    closeSocket(session.clientSocket); /* Close sockets once threads have shutdown */
}


void ConnectionManager::connectionLoop(ClientSession &session)
{
    Logger::instance().log("Starting connection loop for client socket " + std::to_string(session.clientSocket));

    char messageBuffer[2048];

    while (session.active) /* Run for session lifetime */
    {
        /* Wipe buffer */
        memset(messageBuffer, 0, 2048);

        long nBytesRead = recv(session.clientSocket, messageBuffer, 2047, 0);

        if (nBytesRead == 0)
        {
            /* Orderly shutdown. We can close the connection loop */
            Logger::instance().log("Client connection has closed => shutting down connection loop thread");

            removeClientSession(session.clientSocket); /* Safe to call because we're using joinable() */
            return;
        }
        else if (nBytesRead == (-1))
        {
            Logger::instance().log("An error occurred in recv() from client socket " + std::to_string(session.clientSocket));
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
}


void ConnectionManager::senderLoop(ClientSession &session)
{
    Logger::instance().log("Starting sender loop for client socket " + std::to_string(session.clientSocket));

    while (session.active)
    {
        std::unique_lock lock(session.outgoingMutex);
        session.outgoingCV.wait(lock, [this, &session]()
        {
            return (!session.active || !session.outgoingMsgQueue.empty()); /* No longer active or messages to send */
        });

        if (!session.active)
        {
            Logger::instance().log("Session is now inactive. Shutting-down senderLoop");
            return;
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
        Logger::instance().error("Failed to close socket " + std::to_string(socket));
    }
    else
    {
        Logger::instance().info("Closed socket " + std::to_string(socket));
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