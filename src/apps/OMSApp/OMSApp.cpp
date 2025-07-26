/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Socket/Server.hpp"
#include <string>

/**
 * Main entry-point for the OrderManagementSystem
 */
int main(void)
{
    /* Basic version just to test we can receive messages from client */
    Socket::Server theServer(8080);

    theServer.poll();

    return 0;
}