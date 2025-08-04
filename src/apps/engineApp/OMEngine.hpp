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

protected:
    using FixServer::connectToServer; /* Protect since we have the exchange, DB methods */

    /* 35=8 */
    void handleClientFixMessage(FixMessage fixMsg);

    /* 35=AR */
    void handleExchangeAck(FixMessage fixMsg);

    /* 35=AE */
    void handleExchangeFill(FixMessage fixMsg);

private:
    void handleFixMessage(FixMessage fixMsg, SocketFD senderSocket) final;

    /* Store the DB and Exchange connection sockets here for sending messages to right destination */
    SocketFD _exchangeSocket{-1};

    class OrderState
    {
    public:
        struct Order
        {
            /* Key tags from the FIX message */

            enum Status
            {
                PendingNew, /* Received 35=8 message */
                New,        /* Received 35=AR message */
                Fill,       /* Received 35=AE message */
            };

            FixMessage clientFix;

            int side;
            int qty;
            double price;
            std::string currency;
            /* TODO: - add product, originator tags, etc. */

            Status orderStatus{PendingNew};
        };

        void updateOrder(const FixMessage &fix); /* Updates using new FIX type */

    private:
        /* Maps from ClOrdID (11) --> Order */
        std::unordered_map<std::string, Order> _clientOrderMap;
    };
};
