/**
 * @file FixServer.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "FixServer.hpp"
#include "Events/EventLogger.hpp"


FixServer::FixServer(Port serverPort) : Server(serverPort)
{
}


void FixServer::handleMessage(Message clientMessage, SocketFD clientSocket)
{
    /* Construct FIX message and pass down to handleFixMessage() */
    Logger::instance().log("Received FixMsg (source: " + std::to_string(clientSocket) + "): " + clientMessage);
    handleFixMessage(FixMessage(std::move(clientMessage)), clientSocket);
}


void FixServer::sendFixMessage(FixMessage message, SocketFD clientSocket)
{
    /* TODO: - set tag 52 on outgoing message to sending time */

    Logger::instance().log("Sent FixMsg (destination: " + std::to_string(clientSocket) + "): " + message.toString());
    sendMessage(message.toString(), clientSocket);
}
