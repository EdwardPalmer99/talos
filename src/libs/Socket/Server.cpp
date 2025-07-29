/**
 * @file Server.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Server.hpp"
#include "Events/EventLogger.hpp"
#include <cstring>
#include <iostream>
#include <memory>
#include <poll.h>
#include <stdexcept>
#include <thread>
#include <unistd.h>


Server::Server(Port port) : _port(port)
{
    /* Create server socket */
    if ((_listeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == (-1))
    {
        throw std::runtime_error("failed to create server socket");
    }

    /* Setup server address */
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;         /* IPV4 */
    serverAddress.sin_port = htons(_port);      /* Port */
    serverAddress.sin_addr.s_addr = INADDR_ANY; /* Accept all incoming messages */

    /* Bind server socket */
    if ((bind(_listeningSocket, (const struct sockaddr *)&serverAddress, sizeof(serverAddress))) == (-1))
    {
        throw std::runtime_error("failed to bind server socket to address");
    }

    /* Start listening */
    if (listen(_listeningSocket, SOMAXCONN) == (-1))
    {
        throw std::runtime_error("failed to listen on socket");
    }

    /* Create thread for processing received messages */
    _receivedMessageLoopThread = std::thread(&Server::receivedMessageQueueLoop, this);

    /* Start listening and accepting connections */
    _listenLoopThread = std::thread(&Server::listenLoop, this);
}


Server::~Server()
{
    if (close(_listeningSocket) == (-1))
    {
        Logger::instance().log("failed to close socket", Logger::Error);
    }
}


void Server::wait()
{
    if (_receivedMessageLoopThread.joinable())
        _receivedMessageLoopThread.join();
    if (_listenLoopThread.joinable())
        _listenLoopThread.join();
}


void Server::listenLoop()
{
    Logger::instance().log("Starting listen loop...");

    /* Reference: https://man7.org/linux/man-pages/man2/poll.2.html */
    struct pollfd fds;

    fds.fd = _listeningSocket;
    fds.events = POLLIN; /* POLLIN: data to read */

    while (true)
    {
        int pollResult = poll(&fds, 1, 1000); /* 1000ms between polls */

        if (pollResult == 0)
        {
            /* System call timed out with no new connections */
        }
        else if (pollResult == (-1))
        {
            Logger::instance().log("polling error occurred", Logger::Error);
        }
        else
        {
            if (fds.revents & POLLIN) /* New incoming connection */
            {
                int clientSocket = accept(_listeningSocket, nullptr, nullptr);
                if (clientSocket == (-1))
                {
                    Logger::instance().log("failed to accept connection from client", Logger::Error);
                }

                /* Create and detach a new thread to handle messages from the client */
                Logger::instance().log("accepted connection from client on socket " + std::to_string(clientSocket));

                auto thread = std::thread(&Server::connectionLoop, this, clientSocket);
                thread.detach(); /* Behaves independently to server ?? */
            }
        }
    }
}


void Server::connectionLoop(SocketFD clientSocket)
{
    Logger::instance().log("Starting connection loop for client socket " + std::to_string(clientSocket));

    if (clientSocket == (-1))
    {
        Logger::instance().log("invalid client socket", Logger::Error);
        return;
    }

    char messageBuffer[2048];

    while (true)
    {
        /* Wipe buffer */
        memset(messageBuffer, 0, 2048);

        long nBytesRead = recv(clientSocket, messageBuffer, 2047, 0);

        if (nBytesRead == 0)
        {
            /* Orderly shutdown. We can close the connection loop */
            Logger::instance().log("client connection has closed. Shutting down connection loop thread");
            return;
        }
        else if (nBytesRead == (-1))
        {
            Logger::instance().log("can error occurred in recv() from client socket " + std::to_string(clientSocket));
            /* Continue for now */
        }
        else
        {
            Logger::instance().log("received message from client socket " + std::to_string(clientSocket));

            {
                std::unique_lock lock(_receivedMessageMutex);
                _receivedMessageQueue.push(std::string(messageBuffer, nBytesRead));
            }

            _conditionVariable.notify_one(); /* Notify the message queue loop to handle the received message */
        }
    }
}


void Server::receivedMessageQueueLoop()
{
    Logger::instance().log("Starting received message queue loop...");

    while (true)
    {
        std::unique_lock lock(_receivedMessageMutex); /* Wait until we have messages in the queue */
        _conditionVariable.wait(lock, [this]()
        {
            return !_receivedMessageQueue.empty();
        });

        handleClientMessage(_receivedMessageQueue.front());
        _receivedMessageQueue.pop();
    }
}
