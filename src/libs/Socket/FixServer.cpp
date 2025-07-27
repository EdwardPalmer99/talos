/**
 * @file FixServer.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "FixServer.hpp"

FixServer::FixServer(uint16_t serverPort) : Server(serverPort)
{
}


void FixServer::handleClientMessage(std::string message)
{
    /* Construct Fix message and pass to handleFixMessage */
    handleFixMessage(FixMessage(std::move(message)));
}
