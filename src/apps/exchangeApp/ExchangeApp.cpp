/**
 * @file ExchangeApp.cpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Exchange.hpp"
#include "Socket/ConnectionPorts.hpp"


int main(void)
{
    Exchange exchange{ConnectionPorts::ExchangePort};
    // exchange.connectToServer(ConnectionPorts::OMEnginePort);
    exchange.wait();

    return 0;
}