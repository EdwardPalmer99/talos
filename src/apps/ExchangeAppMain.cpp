/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "exchange/ExchangeServer.hpp"
#include <iostream>


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " [PORT]" << std::endl;
        std::cout << "Run an exchange server on the specified port." << std::endl;
        return 0;
    }

    int exchangePort = std::atoi(argv[1]);
    if (!exchangePort)
    {
        std::cerr << argv[0] << ": invalid port " << argv[1] << std::endl;
        return 1;
    }

    ExchangeServer exchange(static_cast<Server::Port>(exchangePort));
    exchange.start();
    exchange.wait();
    return 0;
}