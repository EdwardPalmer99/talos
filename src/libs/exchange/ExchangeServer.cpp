/**
 * @file Exchange.cpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ExchangeServer.hpp"
#include "logger/Logger.hpp"
#include <chrono>


void ExchangeServer::handleFixMessage(FixMessage clientFix, SocketFD clientSocket)
{
    std::string msgType(clientFix.getValue(FixTag::MsgType));

    if (msgType == "QR")
    {
        handleNetAdminCmd(std::move(clientFix), clientSocket);
    }
    else if (msgType == "D") /* TODO: extend to corrections/cancellations */
    {
        sendPartialFill(clientFix, clientSocket);
        sendFill(clientFix, clientSocket);
    }
    else
    {
        Logger::instance().error("Received unsupported msgType [" + msgType + "] => dropping");
    }
}

void ExchangeServer::sendPartialFill(FixMessage clientFix, SocketFD clientSocket)
{
    /* Construct 35=AR message */
    FixMessage ackMessage = std::move(clientFix);
    ackMessage.setTag(FixTag::MsgType, "8");
    ackMessage.setTag(FixTag::ExecType, "1");
    ackMessage.setTag(FixTag::OrdStatus, "1");
    /* TODO: - set remaining tags */

    sendFixMessage(std::move(ackMessage), clientSocket);
}


void ExchangeServer::sendFill(FixMessage clientFix, SocketFD clientSocket)
{
    /* Construct 35=AE message */
    FixMessage fillMessage = std::move(clientFix);
    fillMessage.setTag(FixTag::MsgType, "8");
    fillMessage.setTag(FixTag::ExecType, "2"); /* Fill */
    fillMessage.setTag(FixTag::OrdStatus, "2");
    /* TODO: - set remaining tags */

    sendFixMessage(std::move(fillMessage), clientSocket);
}