/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "client/ClientApp.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " [ENGINE_PORT]" << std::endl;
        std::cout << "Send Fix messages to a Talos OMEngine on the specified port." << std::endl;
        return 0;
    }

    int enginePort = std::atoi(argv[1]);
    if (!enginePort)
    {
        std::cerr << argv[0] << ": invalid port " << argv[1] << std::endl;
        return 1;
    }

    ClientApp app(enginePort, 1000);
    app.start();
    app.wait();

    return 0;
}