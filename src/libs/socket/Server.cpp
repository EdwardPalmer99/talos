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


void Server::onStartup()
{
    Logger::instance().info("Starting up server on port: " + std::to_string(port()));

    /* Create server socket */
    if ((_listeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == (-1))
    {
        throw std::runtime_error("Failed to create server socket");
    }

    /* Enable reuse of a socket (avoid address already in use) */
    int state{1};
    if (setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEADDR, &state, sizeof(state)) == (-1))
    {
        throw std::runtime_error("Failed to set option SO_REUSEADDR");
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

    /* Start listening and accepting connections */
    _listenLoopThread = std::thread(&Server::listenLoop, this);
}


void Server::onWait()
{
    if (_listenLoopThread.joinable())
        _listenLoopThread.join();
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