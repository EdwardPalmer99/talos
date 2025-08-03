/**
 * @file Exchange.hpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "Fix/FixMessage.hpp"
#include "Socket/FixServer.hpp"


class ExchangeFixServer : public FixServer
{
public:
    ExchangeFixServer(uint16_t exchangePort) : FixServer(exchangePort) {}

protected:
    /* 35=8 */
    void handleFixMessage(FixMessage clientFix, SocketFD clientSocket) final;

    /* 35=AR */
    void sendTechAck(FixMessage message, SocketFD clientSocket);

    /* 35=AE */
    void sendFill(FixMessage message, SocketFD clientSocket);
};