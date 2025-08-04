/**
 * @file DatabaseAppMain.cpp
 * @author Edward Palmer
 * @date 2025-08-04
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "database/DatabaseServer.hpp"
#include "socket/ConnectionPorts.hpp"


/* The OrderManager Database for storing OrderRecords */
int main(void)
{
    DatabaseServer database(ConnectionPorts::DatabasePort);
    database.start();
    database.wait();
    return 0;
}