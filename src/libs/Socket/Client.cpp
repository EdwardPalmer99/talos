/**
 * @file Client.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Client.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>


Client::Client(uint16_t serverPort)
{
    /* Server address */
    _serverAddress.sin_family = AF_INET;         /* IPV4 */
    _serverAddress.sin_port = htons(serverPort); /* Port */
    _serverAddress.sin_addr.s_addr = INADDR_ANY; /* Accept all incoming messages */

    /* Client socket */
    if ((_clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == (-1))
    {
        throw std::runtime_error("failed to create client socket");
    }

    /* Establish a connection to the server */
    // TODO: - try this in some sort of loop with a timeout
    if (!doConnect())
    {
        throw std::runtime_error("failed to connect to server");
    }
}


Client::~Client()
{
    /* Close an active connection */
    (void)doDisconnect();
}


bool Client::doConnect()
{
    int result = connect(_clientSocket, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress));
    if (result == (-1))
    {
        /* TODO: - use thread-safe logger here */
        std::cerr << "failed to connect" << std::endl;
        return false;
    }

    return true;
}


bool Client::doDisconnect()
{
    int result = close(_clientSocket);
    if (result == (-1))
    {
        std::cerr << "failed to close socket" << std::endl;
        return false;
    }

    return true;
}


bool Client::doSend(std::string message)
{
    int result = send(_clientSocket, message.c_str(), message.size(), 0);
    if (result == (-1))
    {
        std::cerr << "failed to send message: " << message << std::endl;
        return false;
    }

    /* TODO: - verify bytes here */

    return true;
}
