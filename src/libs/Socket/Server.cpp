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

    int clientSocket{-1};

    while (true)
    {
        /* TODO: - handle connection close and kill thread */
        if (poll(&fds, 1, 1000) != (-1)) /* 1000ms between polls */ // TODO: - fix this block of code
        /* GOOD REFERENCE: https://www.ibm.com/docs/en/i/7.1.0?topic=designs-using-poll-instead-select */
        {
            if (fds.revents & POLLIN)
            {
                if ((clientSocket = accept(_listeningSocket, nullptr, nullptr)) != (-1))
                {
                    Logger::instance().log("received connection request from client on socket " + std::to_string(clientSocket));

                    Logger::instance().log("accepted connection on client socket " + std::to_string(clientSocket), Logger::Info);
                    std::thread(&Server::connectionLoop, this, clientSocket);
                }
            }
        }
        else
        {
            Logger::instance().log("no connection request from client");
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

    const unsigned int kMessageBufferSize = 2048;

    auto messageBuffer = std::make_unique<char>(kMessageBufferSize);

    while (true)
    {
        if ((recv(clientSocket, messageBuffer.get(), kMessageBufferSize - 1, 0)) != (-1)) /* Blocking call */
        {
            Logger::instance().log("received message from client socket " + std::to_string(clientSocket));

            {
                std::unique_lock lock(_receivedMessageMutex);
                _receivedMessageQueue.push(std::string(messageBuffer.get()));
            }

            _conditionVariable.notify_one();
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
