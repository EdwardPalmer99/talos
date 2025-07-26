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

// TODO: - setup to run on a background thread

namespace Socket
{

/* TODO: - create FixClient subclass for sending Fix messages */
class Client
{
public:
    Client() = delete;
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    explicit Client(uint16_t serverPort);

    virtual ~Client();

    /* Assumes that already have an active connection setup */
    bool doSend(std::string message);

protected:
    /* Start a connection to the server. Returns true on success */
    bool doConnect();

    /* Called in destructor */
    bool doDisconnect();

private:
    int _clientSocket{-1};      /* Socket of client (-1 on error) */
    sockaddr_in _serverAddress; /* Address we send packets to */
};

} // namespace Socket