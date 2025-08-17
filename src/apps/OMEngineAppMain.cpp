/**
 * @file OMEngine.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "engine/OMEngine.hpp"
#include <cstring>
#include <iostream>


/**
 * Main entry-point for the Order Management System (OMS)
 */
int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        std::cout << "Usage: " << argv[0] << "[--engine PORT] [--exchange EXCHANGE_PORT] [--database DB_PORT]" << std::endl;
        std::cout << "Run a Talos OMEngine server on the specified port." << std::endl;
        return 0;
    }

    int enginePort{0}, exchangePort{0}, databasePort{0};

    for (int i = 1; i < argc; i += 2)
    {
        if (std::strcmp(argv[i], "--engine") == 0)
            enginePort = atoi(argv[i + 1]);
        else if (std::strcmp(argv[i], "--exchange") == 0)
            exchangePort = atoi(argv[i + 1]);
        else if (std::strcmp(argv[i], "--database") == 0)
            databasePort = atoi(argv[i + 1]);
    }

    /* Verify */
    if (!enginePort || !exchangePort || !databasePort)
    {
        std::cerr << argv[1] << ": invalid/missing port(s)" << std::endl;
        return 1;
    }

    /* TODO: - enable connections to multiple exchanges (can figure-out by tag 100 [named exchange]) */

    OMEngine engineServer(static_cast<Server::Port>(enginePort));
    engineServer.start();
    engineServer.connectToExchangeServer(static_cast<Server::Port>(exchangePort));
    engineServer.connectToDatabaseServer(static_cast<Server::Port>(databasePort));
    engineServer.wait();
    return 0;
}