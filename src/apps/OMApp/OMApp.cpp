/**
 * @file OMEngine.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "OMEngine.hpp"
#include <string>

/**
 * Main entry-point for the Order Management System (OMS)
 */
int main(void)
{
    // TODO: - need someway to handle connections down or not found
    // These should get put into a message queue and wait until up
    // Also write to disk to handle restarts


    /* Basic version just to test we can receive messages from client */
    OMEngine engineServer(8080, 8081);

    engineServer.start();

    return 0;
}