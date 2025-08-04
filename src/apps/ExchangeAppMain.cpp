/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "exchange/ExchangeServer.hpp"
#include "socket/ConnectionPorts.hpp"

int main(void)
{
    ExchangeServer exchange(ConnectionPorts::ExchangePort);
    exchange.start();
    exchange.wait();
    return 0;
}