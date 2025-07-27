/**
 * @file FixClient.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "FixClient.hpp"

FixClient::FixClient(uint16_t serverPort) : Client(serverPort)
{
}


bool FixClient::doSend(const FixMessage &message)
{
    return Client::doSend(message.toString());
}
