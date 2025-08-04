/**
 * @file Exchange.hpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "fix/FixMessage.hpp"
#include "socket/FixServer.hpp"


class ExchangeServer : public FixServer
{
public:
    ExchangeServer(uint16_t exchangePort) : FixServer(exchangePort) {}

protected:
    /* 35=D,... */
    void handleFixMessage(FixMessage clientFix, SocketFD clientSocket) final;

    bool isValidMsgType(const std::string &msgType) const;

    void sendPartialFill(FixMessage message, SocketFD clientSocket);
    void sendFill(FixMessage message, SocketFD clientSocket);
};