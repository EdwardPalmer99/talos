/**
 * @file Exchange.cpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ExchangeFixServer.hpp"
#include "logger/Logger.hpp"
#include <chrono>


void ExchangeFixServer::handleFixMessage(FixMessage clientFix, SocketFD clientSocket)
{
    std::string msgType = clientFix.getValue(FixTag::MsgType);
    if (msgType != "8")
    {
        Logger::instance().log("Received unsupported msgType [" + msgType + "] => dropping", Logger::Error);
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); /* add slight delay */
    sendTechAck(clientFix, clientSocket);

    std::this_thread::sleep_for(std::chrono::microseconds(100));
    sendFill(clientFix, clientSocket);
}


void ExchangeFixServer::sendTechAck(FixMessage clientFix, SocketFD clientSocket)
{
    /* Construct 35=AR message */
    FixMessage ackMessage = std::move(clientFix);
    ackMessage.setTag(FixTag::MsgType, "AR");
    ackMessage.setTag(FixTag::ExecType, "0");

    Logger::instance().log("Sent Tech Ack for ClOrdID: " + ackMessage.getValue(FixTag::ClOrdID));
    sendFixMessage(std::move(ackMessage), clientSocket);
}


void ExchangeFixServer::sendFill(FixMessage clientFix, SocketFD clientSocket)
{
    /* Construct 35=AE message */
    FixMessage fillMessage = std::move(clientFix);
    fillMessage.setTag(FixTag::MsgType, "AE");
    fillMessage.setTag(FixTag::ExecType, "2"); /* Fill */

    Logger::instance().log("Sent Fill for ClOrdID: " + fillMessage.getValue(FixTag::ClOrdID));
    sendFixMessage(std::move(fillMessage), clientSocket);
}