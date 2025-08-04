/**
 * @file OMEngine.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "OMEngine.hpp"
#include "logger/Logger.hpp"
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>


bool OMEngine::connectToExchangeServer(Port exchangePort)
{
    bool ok = connectToServer(exchangePort);
    if (ok)
    {
        _exchangeSocket = _portSocketMappings.getSocket(exchangePort);
    }

    return ok;
}


/* TODO: - add alerting manager on separate instance */
/* TODO: - Client trade booking application needs to stamp unique ClOrdID on messages */
/* TODO: - OMEngine also needs to check for no response from Exchange and trigger an alert */
/* TODO: - Add-in an OMRouter to enable RoundRobin routing to multiple OMEngines */

void OMEngine::handleFixMessage(FixMessage fixMsg, SocketFD senderSocket)
{
    std::string msgType = fixMsg.getValue(FixTag::MsgType);

    /* TODO: - write special enum to convert string to enum */
    if (msgType == "8")
        return handleClientFixMessage(std::move(fixMsg));
    else if (msgType == "AR")
        return handleExchangeAck(std::move(fixMsg));
    else if (msgType == "AE")
        return handleExchangeFill(std::move(fixMsg));

    Logger::instance().log("Invalid message type [" + msgType + "]", Logger::Error);
    /* TODO: - handle; send alert to OMAlert to notify humans */
}


void OMEngine::handleClientFixMessage(FixMessage message)
{
    /* TODO: - Send a message to our DB */
    /* TODO: - stamp some additional tags on such as senderID and send to exchange */

    /**
     *
     * TODO: - add a Router class to abstract away which destination to send to
     */

    message.setTag(FixTag::Trace, message.getValue(FixTag::Trace) + "/OMEngine");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    /* Route to exchange */
    sendFixMessage(message, _exchangeSocket);
}


void OMEngine::handleExchangeAck(FixMessage message)
{
    Logger::instance().log("Processing Exchange Ack. for ClOrdID " + message.getValue(FixTag::ClOrdID));
    /* TODO: - send a 35=UETR message to DB with key info */
    /* TODO: - keep a track somewhere of order status locally --> match by ClOrdID in message --> update DB on status */
    /* DB: ClOrdID price qty product ... status */
}


void OMEngine::handleExchangeFill(FixMessage message)
{
    Logger::instance().log("Processing Exchange Fill for ClOrdID " + message.getValue(FixTag::ClOrdID));

    /* TODO: - send a 35=UETR message to DB with key info */
}
