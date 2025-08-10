/**
 * @file Client.hpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

/* TODO: - have a thread and an outgoing message queue */

class Client
{
public:
    using Port = uint16_t;

    Client();
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    virtual ~Client();

    /* Create a new connection to a server */
    bool connectToServer(Port serverPort);

    /* Thread-safe broadcast a message to all connections */
    bool broadcast(std::string message);

    // TODO: - write a sender loop and retry in case we cannot connect

    /* Accessor for client's socket */
    [[nodiscard]] inline int clientSocket() const { return _clientSocket; }

private:
    int _clientSocket{-1}; /* Socket of client (-1 on error) */
    std::mutex _mutex;     /* Ensures broadcast() threadsafe to avoid corruptions */
};
