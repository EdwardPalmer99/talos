/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "client/OrderGenerator.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " [ENGINE_PORT]" << std::endl;
        std::cout << "Send Fix messages to a Talos OMEngine on the specified port." << std::endl;
        return 0;
    }

    /* TODO: - add additional options to connect to multiple engines, use broadcast to send messages to all */
    /* TODO: - add option for # messages to be sent and interval */

    int enginePort = std::atoi(argv[1]);
    if (!enginePort)
    {
        std::cerr << argv[0] << ": invalid port " << argv[1] << std::endl;
        return 1;
    }

    OrderGenerator orderGeneratorClient;
    orderGeneratorClient.start();
    orderGeneratorClient.connectToServer(static_cast<Client::Port>(enginePort));
    orderGeneratorClient.sendNewOrders(1000);
    orderGeneratorClient.wait();
    return 0;
}