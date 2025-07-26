/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Socket/Client.hpp"
#include <iostream>
#include <string>

/* TODO: - create Trading application subclass of Client with different operations */

/**
 * Front-end of the client application for traders
 *
 * ClientApp (new, cancel, correct) ---> OrderManagementSystem
 *
 */
int main(void)
{
    // TODO: - each trade sent by a client should have a unique OrderID for the DataBase so we can lookup
    // records of actions done

    std::cout << "Setting up client..." << std::endl;

    /* Create our client with the OMS server host */
    Socket::Client theClient(8080);

    // TODO: - run the client on a background thread and add a logger

    std::cout << "Ready to send messages to OMS..." << std::endl;

    std::string line;
    while (std::getline(std::cin, line))
    {
        /* TODO: - set a message based on the line. TODO: - add a nice UI with different commands */
        std::cout << "Sending message to OMS: " << line << std::endl;

        (void)theClient.doSend(line);
    }

    return 0;
}