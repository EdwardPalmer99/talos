/**
 * @file OMEngine.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "engine/OMEngine.hpp"
#include "socket/ConnectionPorts.hpp"
#include <string>

/**
 * Main entry-point for the Order Management System (OMS)
 */
int main(void)
{
    // TODO: - need someway to handle connections down or not found
    // These should get put into a message queue and wait until up
    // Also write to disk to handle restarts


    OMEngine engineServer(ConnectionPorts::OMEnginePort);
    engineServer.start();
    engineServer.connectToExchangeServer(ConnectionPorts::ExchangePort);
    engineServer.connectToDatabaseServer(ConnectionPorts::DatabasePort);
    engineServer.wait();
    return 0;
}