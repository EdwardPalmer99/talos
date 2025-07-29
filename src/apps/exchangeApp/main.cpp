/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ExchangeFixServer.hpp"
#include "Socket/ConnectionPorts.hpp"


int main(void)
{
    ExchangeFixServer exchange(ConnectionPorts::ExchangePort);
    exchange.connectToServer(ConnectionPorts::OMEnginePort);

    exchange.wait();
    return 0;
}