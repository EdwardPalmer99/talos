/**
 * @file NetAdminAppMain.cpp
 * @author Edward Palmer
 * @date 2025-08-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "netadmin/NetAdmin.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
    if (argc != 3 && argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " [PORT] [CMD]" << std::endl;
        std::cout << "Send admin commands to the specified application." << std::endl;
        return 0;
    }

    int thePort = std::atoi(argv[1]);
    if (!thePort)
    {
        std::cerr << argv[0] << ": invalid port " << argv[1] << std::endl;
        return 1;
    }

    NetAdmin adminClient;
    adminClient.start();
    adminClient.connectToServer(static_cast<Client::Port>(thePort));

    adminClient.sendAdminCommand(std::string(argv[2]));

    adminClient.stop();
    adminClient.wait();
    return 0;
}