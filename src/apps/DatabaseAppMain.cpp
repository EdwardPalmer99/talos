/**
 * @file DatabaseAppMain.cpp
 * @author Edward Palmer
 * @date 2025-08-04
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "database/DatabaseServer.hpp"
#include <iostream>
#include <string>


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " [PORT]" << std::endl;
        std::cout << "Run a Talos OMDatabase on the specified port." << std::endl;
        return 0;
    }

    int databasePort = std::atoi(argv[1]); /* Returns 0 on failure */
    if (!databasePort)
    {
        std::cerr << argv[0] << ": invalid port " << argv[1] << std::endl;
        return 1;
    }

    DatabaseServer database(static_cast<Server::Port>(databasePort));
    database.start();
    database.wait();

    return 0;
}