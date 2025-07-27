/**
 * @file FixClient.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "Client.hpp"
#include "Fix/FixMessage.hpp"
#include "Fix/FixTag.hpp"

class FixClient : protected Client
{
public:
    FixClient() = delete;
    FixClient(uint16_t serverPort);

    bool doSend(const FixMessage &message);
};
