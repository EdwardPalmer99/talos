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
    /* Basic version just to test we can receive messages from client */
    OMEngine engineServer(8080);

    engineServer.start();

    return 0;
}