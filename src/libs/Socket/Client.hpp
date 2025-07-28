/**
 * @file Client.hpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

class Client
{
public:
    Client();
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    virtual ~Client();

    /* Create a new connection to a server */
    bool connectToServer(int serverPort);

    /* Broadcast a message to all connections */
    bool broadcast(std::string message);

    /* Accessor for client's socket */
    [[nodiscard]] inline int clientSocket() const { return _clientSocket; }

private:
    int _clientSocket{-1}; /* Socket of client (-1 on error) */
};
