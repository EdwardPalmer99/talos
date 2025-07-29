/**
 * @file OMEngine.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "OMEngine.hpp"
#include "Events/EventLogger.hpp"
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

/* TODO: - add alerting manager on separate instance */
/* TODO: - Client trade booking application needs to stamp unique ClOrdID on messages */
/* TODO: - OMEngine also needs to check for no response from Exchange and trigger an alert */
/* TODO: - Add-in an OMRouter to enable RoundRobin routing to multiple OMEngines */

void OMEngine::handleFixMessage(FixMessage message)
{
    std::string msgType = message.getValue(FixTag::MsgType);

    if (msgType == "8")
        return handleClientFixMessage(std::move(message));
    else if (msgType == "AR")
        return handleExchangeAck(std::move(message));
    else if (msgType == "AE")
        return handleExchangeFill(std::move(message));

    /* TODO: - handle; send alert to OMAlert to notify humans */
    throw std::runtime_error("received invalid msgType [" + msgType + "]");
}


void OMEngine::handleClientFixMessage(FixMessage message)
{
    /* TODO: - Send a message to our DB */
    /* TODO: - stamp some additional tags on such as senderID and send to exchange */

    Logger::instance().log("OMEngine received ClientFix: [" + message.toString() + "]");

    message.setTag(FixTag::Trace, message.getValue(FixTag::Trace) + "/OMEngine");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (!broadcast(message)) /* Route down to exchange */
    {
        Logger::instance().log("failed to send message to exchange for ClOrdID [" + message.getValue(FixTag::ClOrdID) + "]");
        return;
    }

    Logger::instance().log("sent message to exchange for ClOrdID [" + message.getValue(FixTag::ClOrdID) + "]");
}


void OMEngine::handleExchangeAck(FixMessage message)
{
    Logger::instance().log("OMEngine received ExchangeAck: [" + message.toString() + "]");


    /* TODO: - send a 35=UETR message to DB with key info */
}


void OMEngine::handleExchangeFill(FixMessage message)
{
    Logger::instance().log("OMEngine received ExchangeFill: [" + message.toString() + "]");

    /* TODO: - send a 35=UETR message to DB with key info */
}
