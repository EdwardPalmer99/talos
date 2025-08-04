/**
 * @file OMEngine.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "fix/FixMessage.hpp"
#include "socket/FixClient.hpp"
#include "socket/FixServer.hpp"
#include <shared_mutex>
#include <unordered_map>

// TODO: - be smarter with a Queue when we receive messages
/**
 * Order Management System Engine
 *
 * 1. Receive 35=8 message from client booking application
 * 2. Send 35=8 message downstream to exchange and to DB ==> New
 * 3. Receive 35=AR exchange trade capture report acknowledgement (update DB) ==> PendingFill
 * 4. Receive 35=AE exchange trade capture report (update DB) ==> Fill
 *
 * Simplified Diagram of message-flow:
 *
 * HighTouchApp --> OMEngine --> OMDatabase
 *                      |
 *                   Exchange
 */
class OMEngine : public FixServer
{
public:
    OMEngine() = delete;

    OMEngine(Port enginePort) : FixServer(enginePort) {}

    /* Setup connection to exchange server. Should be called after start() and before wait() */
    bool connectToExchangeServer(Port exchangePort);

    bool connectToDatabaseServer(Port databasePort);

protected:
    using FixServer::connectToServer; /* Protect since we have the exchange, DB methods */

    /* 35=D */
    void handleClientFixMessage(FixMessage fixMsg, SocketFD senderSocket);

    /* 35=8 */
    void handleExchangeFixMessage(FixMessage fixMsg, SocketFD senderSocket);

    /* 39=1, 150=1 */
    void handleExchangePartialFill(FixMessage fixMsg, SocketFD senderSocket);

    /* 39=2, 150=2 */
    void handleExchangeFill(FixMessage fixMsg, SocketFD senderSocket);

    /* TODO: - handle other exchange states and client cancellation/corrections */

private:
    void handleFixMessage(FixMessage fixMsg, SocketFD senderSocket) final;

    /* Store the DB and Exchange connection sockets here for sending messages to right destination */
    SocketFD _exchangeSocket{-1};
    SocketFD _databaseSocket{-1};

    /* TODO: - bit inefficient and may may slow-down for multiple clients. Can use a tag on the FIX to identify original sender */
    /* Required for correctly routing messages received from the exchange back to the client */
    /* TODO: - encapsulate in a struct */
    std::shared_mutex _clientSocketMutex;
    std::unordered_map<std::string, SocketFD> _clientSocketForClOrdID;

    SocketFD getClientSocket(std::string clientOrdID);

    void updateClientSocketMap(std::string clOrdID, SocketFD clientSocket);

    /* Call when we have completed an order (or it was rejected) */
    void eraseClientSocketMap(std::string clOrdID);
};
