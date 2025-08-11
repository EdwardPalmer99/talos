/**
 * @file Server.hpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "ConnectionManager.hpp"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <shared_mutex>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unordered_map>


class Server : public ConnectionManager
{
public:
    explicit Server(Port port);

    /* Accessor for server's port */
    [[nodiscard]] inline Port port() const { return _port; }

    /* Accessor for server's listening socket */
    [[nodiscard]] inline SocketFD listenSocket() const { return _listeningSocket; }

protected:
    void onStartup() override;
    void onWait() override;

private:
    /* Accept incoming connections and spawn new connection/sender loops */
    void listenLoop();

    Port _port;                    /* Server's port */
    SocketFD _listeningSocket{-1}; /* Server socket's file-descriptor (FD). (-1) on error */

    std::thread _listenLoopThread;
};
