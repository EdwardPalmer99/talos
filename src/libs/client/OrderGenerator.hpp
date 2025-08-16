/**
 * @file OrderGenerator.hpp
 * @author Edward Palmer
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "fix/FixMessage.hpp"
#include "socket/FixClient.hpp"


/* Generates dummy FIX orders for stress-testing an OMEngine */
class OrderGenerator : public FixClient
{
public:
    void sendNewOrders(std::size_t nOrders, std::size_t delayMS = 0);

protected:
    /* Note: Not handling any incoming messages currently */
    void handleFixMessage(FixMessage, SocketFD) final{};

    FixMessage buildNewOrder();

    /* Add correct, cancel methods to construct those message with message ID */
};