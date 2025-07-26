/**
 * @file Server.hpp
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


namespace Socket
{

class Server
{
public:
    Server() = delete;
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    explicit Server(uint16_t serverPort);

    virtual ~Server();

    /* Called when we receive a message from the client */
    virtual void handleClientMessage(std::string message);

    /* Polling for client messages TODO: - improve this */
    void poll();

protected:
    /* Called in destructor */
    bool doDisconnect();

private:
    int _clientSocket{-1};
    int _serverSocket{-1};      /* Socket of Server (-1 on error) */
    sockaddr_in _serverAddress; /* Address we send packets to */
};

} // namespace Socket