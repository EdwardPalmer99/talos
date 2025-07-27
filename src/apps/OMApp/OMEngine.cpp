/**
 * @file OMEngine.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "OMEngine.hpp"
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
    std::string msgType = message.getValue(35);

    if (msgType == "8")
        return handleClientFixMessage(std::move(message));
    else if (msgType == "AR")
        return handleExchangeAck(std::move(message));
    else if (msgType == "AE")
        return handleExchangeFill(std::move(message));

    throw std::runtime_error("received invalid msgType [" + msgType + "]");
}


void OMEngine::handleClientFixMessage(FixMessage message)
{
    /* TODO: - Send a message to our DB */
    /* TODO: - stamp some additional tags on such as senderID and send to exchange */

    /* TODO: - use a logger here */
    std::cout << "OMEngine received ClientFix: [" << message.toString() << "]" << std::endl;
}


void OMEngine::handleExchangeAck(FixMessage message)
{
    std::cout << "OMEngine received ExchangeAck: [" << message.toString() << "]" << std::endl;
}


void OMEngine::handleExchangeFill(FixMessage message)
{
    std::cout << "OMEngine received ExchangeFill: [" << message.toString() << "]" << std::endl;
}
