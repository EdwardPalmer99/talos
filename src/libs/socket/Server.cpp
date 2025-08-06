/**
 * @file Server.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Server.hpp"
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


Server::Server(Port port) : _port(port)
{
}


Server::~Server()
{
    stop();
    wait(); /* Ensure logger has shutdown */
}


void Server::start()
{
    if (_active) /* Already running */
    {
        return;
    }

    /* Startup logger */
    Logger::instance().start();
    Logger::instance().info("Starting server on port " + std::to_string(port()));

    /* Create server socket */
    if ((_listeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == (-1))
    {
        throw std::runtime_error("Failed to create server socket");
    }

    /* Setup server address */
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;         /* IPV4 */
    serverAddress.sin_port = htons(_port);      /* Port */
    serverAddress.sin_addr.s_addr = INADDR_ANY; /* Accept all incoming messages */

    /* Bind server socket */
    if ((bind(_listeningSocket, (const struct sockaddr *)&serverAddress, sizeof(serverAddress))) == (-1))
    {
        throw std::runtime_error("Failed to bind server socket to address");
    }

    /* Start listening */
    if (listen(_listeningSocket, SOMAXCONN) == (-1))
    {
        throw std::runtime_error("Failed to listen on socket");
    }

    /* Set server status to active */
    _active = true;

    /* Create thread for processing received messages */
    _handleMessageLoopThread = std::thread(&Server::handleMessageLoop, this);

    /* Start listening and accepting connections */
    _listenLoopThread = std::thread(&Server::listenLoop, this);
}


void Server::wait()
{
    if (_handleMessageLoopThread.joinable())
        _handleMessageLoopThread.join();
    if (_listenLoopThread.joinable())
        _listenLoopThread.join();

    Logger::instance().wait();
}


void Server::stop()
{
    Logger::instance().log("Initiating server shutdown...");

    if (!_active)
        return;

    _active = false;
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

    /* Shutdown logger */
    Logger::instance().stop();
}


bool Server::connectToServer(Port serverPort)
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


void Server::listenLoop()
{
    Logger::instance().log("Starting listen loop on socket " + std::to_string(_listeningSocket));

    /* Reference: https://man7.org/linux/man-pages/man2/poll.2.html */
    struct pollfd fds;

    fds.fd = _listeningSocket;
    fds.events = POLLIN; /* POLLIN: data to read */

    while (_active) /* Run for server lifetime */
    {
        int pollResult = poll(&fds, 1, 1000); /* 1000ms between polls */

        if (pollResult == 0)
        {
            /* System call timed out with no new connections */
        }
        else if (pollResult == (-1))
        {
            Logger::instance().log("A polling error occurred. Continuing...", Logger::Error);
        }
        else
        {
            if (fds.revents & POLLIN) /* New incoming connection */
            {
                int clientSocket = accept(_listeningSocket, nullptr, nullptr);
                if (clientSocket == (-1))
                {
                    Logger::instance().log("Failed to accept connection from client", Logger::Error);
                }

                /* Create and detach a new thread to handle messages from the client */
                Logger::instance().log("Accepted new connection from client on socket " + std::to_string(clientSocket));
                addClientSession(clientSocket);
            }
        }
    }

    /* Cleanup */
    closeSocket(_listeningSocket);
}


void Server::handleMessageLoop()
{
    Logger::instance().log("Starting handleMessage loop");

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


void Server::addClientSession(SocketFD clientSocket)
{
    if (clientSocket == (-1))
    {
        Logger::instance().log("Invalid client socket (" + std::to_string(clientSocket) + ")", Logger::Error);
        return;
    }

    auto session = std::make_unique<ClientSession>(clientSocket);
    session->active = true;

    session->connectionThread = std::thread(&Server::connectionLoop, this, std::ref(*session));
    session->senderThread = std::thread(&Server::senderLoop, this, std::ref(*session));

    {
        std::unique_lock lock(_clientSessionMutex);
        _clientSessionMap[clientSocket] = std::move(session);
    }
}


void Server::removeClientSession(SocketFD clientSocket)
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


void Server::closeClientSession(ClientSession &session)
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


void Server::connectionLoop(ClientSession &session)
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


void Server::senderLoop(ClientSession &session)
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


/* Add send connection loop (one per client socket) */

/* TODO: - create a separate message queue for different connections */
/* TODO: - crete an outgoing message queue thread */
/* TODO: - share code between client and server */


void Server::sendMessage(Message message, SocketFD clientSocket)
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


void Server::closeSocket(SocketFD socket)
{
    if (close(socket) == (-1))
    {
        Logger::instance().log("Failed to close socket " + std::to_string(socket), Logger::Error);
    }
    else
    {
        Logger::instance().log("Closed socket " + std::to_string(socket), Logger::Info);
    }
}


void Server::PortSocketMappings::clear()
{
    std::unique_lock lock(_portSocketMutex);
    _portToSocket.clear();
    _socketToPort.clear();
}


void Server::PortSocketMappings::update(Port port, SocketFD socket)
{
    std::unique_lock lock(_portSocketMutex);
    _portToSocket[port] = socket;
    _socketToPort[socket] = port;
}


Server::Port Server::PortSocketMappings::getPort(SocketFD socket) const
{
    std::shared_lock lock(_portSocketMutex);

    auto iter = _socketToPort.find(socket);

    return (iter != _socketToPort.end()) ? iter->second : 0;
}


Server::SocketFD Server::PortSocketMappings::getSocket(Port port) const
{
    std::shared_lock lock(_portSocketMutex);

    auto iter = _portToSocket.find(port);

    return (iter != _portToSocket.end()) ? iter->second : (-1);
}


void Server::PortSocketMappings::erase(SocketFD socket)
{
    Port port = getPort(socket);

    std::unique_lock lock(_portSocketMutex); /* Careful with lock since getPort has lock */
    _portToSocket.erase(port);
    _socketToPort.erase(socket);
}