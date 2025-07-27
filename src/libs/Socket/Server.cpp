/**
 * @file Server.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Server.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unistd.h>


Server::Server(uint16_t serverPort)
{
    /* Create server socket */
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* Server address */
    _serverAddress.sin_family = AF_INET;         /* IPV4 */
    _serverAddress.sin_port = htons(serverPort); /* Port */
    _serverAddress.sin_addr.s_addr = INADDR_ANY; /* Accept all incoming messages */

    /* Bind server socket */
    if ((bind(_serverSocket, (const struct sockaddr *)&_serverAddress, sizeof(_serverAddress))) == (-1))
    {
        throw std::runtime_error("failed to bind server socket");
    }

    listen(_serverSocket, 5); /* Socket, N requests to queue before refusal */
    _clientSocket = accept(_serverSocket, nullptr, nullptr);
}


Server::~Server()
{
    doDisconnect();
}


bool Server::doDisconnect()
{
    int result = close(_serverSocket);
    if (result == (-1))
    {
        std::cerr << "failed to close socket" << std::endl;
        return false;
    }

    return true;
}

void Server::start()
{
    char buffer[1024];

    while (true) /* TODO: - Insanely inefficient and bad */
    {
        memset(buffer, 0, sizeof(char) * 1024);
        if ((recv(_clientSocket, buffer, sizeof(buffer), 0)) == (-1))
        {
            /* No message */
        }

        handleClientMessage(std::string(buffer));
        sleep(2);
    }
}
