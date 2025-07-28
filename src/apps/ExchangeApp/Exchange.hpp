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
#include "Socket/FixClient.hpp"
#include "Socket/FixServer.hpp"


class Exchange : public FixServer, public FixClient
{
public:
    Exchange(uint16_t exchangePort) : FixServer(exchangePort) {}

protected:
    /* 35=8 */
    void handleFixMessage(FixMessage message) final;
};