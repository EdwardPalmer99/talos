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
#include <string>


class FixClient : protected Client
{
public:
    FixClient() = delete;
    FixClient(uint16_t serverPort);

    bool doSend(FixMessage &message);

protected:
    std::string sendingTimeUTC() const;
};
