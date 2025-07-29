/**
 * @file Client.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Client.hpp"
#include "Events/EventLogger.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>


Client::Client()
{
    /* Client socket */
    if ((_clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == (-1))
    {
        throw std::runtime_error("failed to create client socket");
    }
}


Client::~Client()
{
    if (close(_clientSocket) == (-1))
    {
        Logger::instance().log("failed to close socket " + std::to_string(_clientSocket), Logger::Error);
    }
}


bool Client::connectToServer(int serverPort)
{
    /* Address of server we would like to connect to */
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;                     /* IPV4 */
    serverAddress.sin_port = htons(serverPort);             /* Port */
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); /* Connect to localhost */

    if (connect(_clientSocket, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)) == (-1))
    {
        Logger::instance().log("failed to connect to server on port " + std::to_string(serverPort));
        return false;
    }

    Logger::instance().log("connected to server on port " + std::to_string(serverPort));
    return true;
}

/* TODO: - write a send queue. So can operate this on any thread and will send on a background thread */


bool Client::broadcast(std::string message)
{
    std::unique_lock lock(_mutex);

    if (send(_clientSocket, message.c_str(), message.size(), 0) == (-1))
    {
        Logger::instance().log("failed to broadcast message", Logger::Error);
        return false;
    }

    Logger::instance().log("sent message: " + message);
    return true;
}
