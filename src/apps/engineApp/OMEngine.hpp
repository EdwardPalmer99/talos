/**
 * @file OMEngine.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "Fix/FixMessage.hpp"
#include "Socket/FixClient.hpp"
#include "Socket/FixServer.hpp"

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
class OMEngine : public FixServer, public FixClient
{
public:
    OMEngine(uint16_t enginePort) : FixServer(enginePort) {}

protected:
    /* 35=8 */
    virtual void handleClientFixMessage(FixMessage message);

    /* 35=AR */
    virtual void handleExchangeAck(FixMessage message);

    /* 35=AE */
    virtual void handleExchangeFill(FixMessage message);

private:
    void handleFixMessage(FixMessage message) final;
};
