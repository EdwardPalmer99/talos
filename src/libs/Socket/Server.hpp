/**
 * @file Server.hpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <condition_variable>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unordered_set>


class Server
{
public:
    using Port = uint16_t;
    using SocketFD = int; /* Socket file descriptor (FD) */

    Server() = delete;
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    explicit Server(Port port);

    virtual ~Server();

    /* Accessor for server's port */
    [[nodiscard]] inline Port port() const { return _port; }

    /* Accessor for server's listening socket */
    [[nodiscard]] inline SocketFD listenSocket() const { return _listeningSocket; }

    /* Block while running */
    void wait();

protected:
    /* Called when we receive a message from the client */
    virtual void handleClientMessage(std::string message) = 0;

private:
    /*
     * Startup server to listen on its port. Accepts incoming connections and creates a new thread to handle each
     * connection.
     */
    void listenLoop();

    /* Once we accept a connection, we create a new thread to receive messages from that connection on the loop */
    void connectionLoop(SocketFD clientSocket);

    /* Calls handleClientMessage() on each message in the received message queue */
    void receivedMessageQueueLoop();

    Port _port;                    /* Server's port */
    SocketFD _listeningSocket{-1}; /* Server socket's file-descriptor (FD). (-1) on error */

    std::condition_variable _conditionVariable;
    std::mutex _receivedMessageMutex;
    std::queue<std::string> _receivedMessageQueue; /* All active messages received from connections */

    std::thread _receivedMessageLoopThread;
    std::thread _listenLoopThread;
};
