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


/* TODO: - add a retry connection loop if we fail to connect the first couple of times (try every few seconds) */

bool OMEngine::connectToExchangeServer(Port exchangePort)
{
    bool ok = connectToServer(exchangePort);
    if (ok)
    {
        _exchangeSocket = _portSocketMappings.getSocket(exchangePort);
    }

    return ok;
}


bool OMEngine::connectToDatabaseServer(Port databasePort)
{
    bool ok = connectToServer(databasePort);
    if (ok)
    {
        _databaseSocket = _portSocketMappings.getSocket(databasePort);
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
    if (msgType == "D")
        return handleClientFixMessage(std::move(fixMsg), senderSocket);
    else if (msgType == "8")
        return handleExchangeFixMessage(std::move(fixMsg), senderSocket);

    Logger::instance().log("Invalid message type [" + msgType + "]", Logger::Error);
    /* TODO: - handle; send alert to OMAlert to notify humans */
}


void OMEngine::handleClientFixMessage(FixMessage clientFixMsg, SocketFD clientSocket)
{
    /* TODO: - add additional subhandling for different message types such as new/correction/cancellation */
    /* NB: currently assume all messages from client are 20=0; 39=0; 150=0; */
    /* TODO: - enable routing to multiple exchanges based on message tags */

    /* Client (35=D) --> OMEngine */
    updateClientSocketMap(clientFixMsg.getValue(FixTag::ClOrdID), clientSocket);

    /* OMEngine --> Exchange, Database (35=D) */
    sendFixMessage(clientFixMsg, _exchangeSocket);
    sendFixMessage(clientFixMsg, _databaseSocket);

    /* OMEngine --> Client, Database (35=8) */
    FixMessage execReport(clientFixMsg);
    execReport.setTag(FixTag::MsgType, "8");
    execReport.setTag(FixTag::OrdStatus, "0"); /* New */

    sendFixMessage(execReport, clientSocket);
    sendFixMessage(execReport, _databaseSocket);
}


void OMEngine::handleExchangeFixMessage(FixMessage exchFixMsg, SocketFD exchangeSocket)
{
    /* TODO: - map to an enum */
    std::string orderStatus(exchFixMsg.getValue(FixTag::OrdStatus));

    if (orderStatus == "1") /* Partial fill */
    {
        handleExchangePartialFill(std::move(exchFixMsg), exchangeSocket);
    }
    else if (orderStatus == "2") /* Fill */
    {
        handleExchangeFill(std::move(exchFixMsg), exchangeSocket);
    }

    /* Handle other cases */
    Logger::instance().error("Not handling exchange message with order status: " + orderStatus);
}


void OMEngine::handleExchangePartialFill(FixMessage exchFixMsg, SocketFD)
{
    /* Exchange (35=8; 39=1) => OMEngine */
    /* OMEngine (35=8) => Client, DB */

    std::string clOrdID(exchFixMsg.getValue(FixTag::ClOrdID));

    /* TODO: - enrich some tags here */
    SocketFD clientSocket = getClientSocket(clOrdID);

    if (clientSocket == (-1))
        Logger::instance().error("No client socket found for clOrdID " + clOrdID);
    else
        sendFixMessage(exchFixMsg, clientSocket);

    sendFixMessage(exchFixMsg, _databaseSocket);
}


void OMEngine::handleExchangeFill(FixMessage exchFixMsg, SocketFD)
{
    /* Exchange (35=8; 39=2) => OMEngine */
    /* OMEngine (35=8) => Client, DB */

    std::string clOrdID(exchFixMsg.getValue(FixTag::ClOrdID));

    /* TODO: - enrich some tags here */
    SocketFD clientSocket = getClientSocket(clOrdID);

    if (clientSocket == (-1))
        Logger::instance().error("No client socket found for clOrdID " + clOrdID);
    else
        sendFixMessage(exchFixMsg, clientSocket);

    sendFixMessage(exchFixMsg, _databaseSocket);
}


OMEngine::SocketFD OMEngine::getClientSocket(std::string clOrdID)
{
    std::shared_lock lock(_clientSocketMutex);

    auto iter = _clientSocketForClOrdID.find(clOrdID);
    return (iter != _clientSocketForClOrdID.end()) ? iter->second : (-1);
}


void OMEngine::updateClientSocketMap(std::string clOrdID, SocketFD clientSocket)
{
    std::unique_lock lock(_clientSocketMutex);
    _clientSocketForClOrdID[clOrdID] = clientSocket;
}