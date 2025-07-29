/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ClientApp.hpp"
#include "Socket/ConnectionPorts.hpp"

int main(void)
{
    ClientApp app(ConnectionPorts::OMEnginePort, 1000);
    app.start();
    app.wait();

    return 0;
}