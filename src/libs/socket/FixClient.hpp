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
#include "fix/FixMessage.hpp"
#include "fix/FixTag.hpp"
#include <atomic>
#include <string>


class FixClient : public Client
{
public:
    FixClient() = default;

    /* Thread-safe broadcast to all servers */
    bool broadcast(FixMessage &message);

protected:
    std::string sendingTimeUTC() const;

private:
    std::atomic<long> _msgSeqNo{1};
};
