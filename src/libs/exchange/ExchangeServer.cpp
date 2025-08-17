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

void ExchangeServer::onRegisterMsgTypes()
{
    FixServer::onRegisterMsgTypes();

    auto handler = [this](FixMessage message, SocketFD socket) -> void
    {
        sendFixMessage(buildPartialFill(message), socket);
        sendFixMessage(buildFill(message), socket);
    };

    registerMsgTypeHandler("D", handler); /* TODO: - extend for corrections/cancellations (different types) */
}


FixMessage ExchangeServer::buildPartialFill(const FixMessage &clientFix) const
{
    /* Construct 35=AR message */
    FixMessage ackMessage = clientFix;
    ackMessage.setTag(FixTag::MsgType, "8");
    ackMessage.setTag(FixTag::ExecType, "1");
    ackMessage.setTag(FixTag::OrdStatus, "1");
    /* TODO: - set remaining tags */

    return ackMessage;
}


FixMessage ExchangeServer::buildFill(const FixMessage &clientFix) const
{
    /* Construct 35=AE message */
    FixMessage fillMessage = clientFix;
    fillMessage.setTag(FixTag::MsgType, "8");
    fillMessage.setTag(FixTag::ExecType, "2"); /* Fill */
    fillMessage.setTag(FixTag::OrdStatus, "2");
    /* TODO: - set remaining tags */

    return fillMessage;
}