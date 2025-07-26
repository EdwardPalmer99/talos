/**
 * @file Server.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Server.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>


namespace Socket
{

Server::Server(uint16_t serverPort)
{
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

void Server::poll()
{
    char buffer[1024];

    while (true) /* TODO: - Insanely inefficient and bad */
    {
        if ((recv(_clientSocket, buffer, sizeof(buffer), 0)) == (-1))
        {
            /* No message */
        }

        handleClientMessage(std::string(buffer));
        sleep(2);
    }
}

void Server::handleClientMessage(std::string message)
{
    std::cout << "Received message: " << message << std::endl;
}


} // namespace Socket