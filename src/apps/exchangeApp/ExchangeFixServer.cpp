/**
 * @file Exchange.cpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ExchangeFixServer.hpp"
#include "Events/EventLogger.hpp"
#include <chrono>


void ExchangeFixServer::handleFixMessage(FixMessage message)
{
    Logger::instance().log("received Fix: [" + message.toString() + "]");

    std::string msgType = message.getValue(FixTag::MsgType);
    if (msgType != "8")
    {
        Logger::instance().log("received invalid msgType [" + msgType + "]", Logger::Error);
        return;
    }

    /* TODO: - Clients should have a queue of outgoing messages */
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); /* add slight delay */
    broadcastTechAck(message);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    broadcastFill(message);
}


void ExchangeFixServer::broadcastTechAck(FixMessage &message)
{
    /* Construct 35=AR message */
    FixMessage ackMessage(message);
    ackMessage.setTag(FixTag::MsgType, "AR");
    ackMessage.setTag(FixTag::ExecType, "0");

    if (!broadcast(ackMessage))
    {
        Logger::instance().log("failed to send technical ack for ClOrdID [" + message.getValue(FixTag::ClOrdID) + "]");
        return;
    }

    Logger::instance().log("sent technical ack for ClOrdID [" + message.getValue(FixTag::ClOrdID) + "]");
}


void ExchangeFixServer::broadcastFill(FixMessage &message)
{
    /* Construct 35=AE message */
    FixMessage fillMessage(message);
    fillMessage.setTag(FixTag::MsgType, "AE");
    fillMessage.setTag(FixTag::ExecType, "2"); /* Fill */

    if (!broadcast(fillMessage))
    {
        Logger::instance().log("failed to send fill for ClOrdID [" + message.getValue(FixTag::ClOrdID) + "]");
        return;
    }

    Logger::instance().log("sent fill for ClOrdID [" + message.getValue(FixTag::ClOrdID) + "]");
}