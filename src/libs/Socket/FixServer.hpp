/**
 * @file FixServer.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "Fix/FixMessage.hpp"
#include "Server.hpp"

class FixServer : public Server
{
public:
    FixServer(Port serverPort);

protected:
    /* To be implemented. What to do when we receive a client Fix */
    virtual void handleFixMessage(FixMessage message, SocketFD clientSocket) = 0;

    /* Send a FIX message to a client */
    void sendFixMessage(FixMessage message, SocketFD clientSocket);

private:
    void handleMessage(Message clientMessage, SocketFD clientSocket) final;

    using Server::sendMessage; /* Make private */
};
