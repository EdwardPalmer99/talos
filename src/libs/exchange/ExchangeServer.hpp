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
    /* Hooks */
    void onRegisterMsgTypes() override;

private:
    /* Message builders */
    FixMessage buildPartialFill(const FixMessage &message) const;
    FixMessage buildFill(const FixMessage &message) const;
};