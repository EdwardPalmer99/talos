/**
 * @file FixServer.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "Server.hpp"
#include "fix/FixMessage.hpp"

class FixServer : public Server
{
public:
    FixServer(Port serverPort);

protected:
    /* To be implemented. What to do when we receive a client Fix */
    virtual void handleFixMessage(FixMessage message, SocketFD clientSocket) = 0;

    /* Send a FIX message to a client */
    void sendFixMessage(FixMessage message, SocketFD clientSocket);

    /* Stamps tags for destination, sending time on sent messages */
    virtual void enrichFixMessage(FixMessage &message);

    /* Current UTC (GMT) time to millisecond precision */
    std::string nowUTC() const;

private:
    void handleMessage(Message clientMessage, SocketFD clientSocket) final;

    using Server::sendMessage; /* Make private */
};
