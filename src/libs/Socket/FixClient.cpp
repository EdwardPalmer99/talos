/**
 * @file FixClient.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "FixClient.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>


bool FixClient::broadcast(FixMessage &message)
{
    /* Set sending time to current time in UTC */
    message.setTag(FixTag::SendingTime, sendingTimeUTC());

    return Client::broadcast(message.toString());
}


std::string FixClient::sendingTimeUTC() const
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::ostringstream os;                                            /* TODO: - use gmtime_r for thread-safe versiono */
    os << std::put_time(gmtime(&currentTime), "%Y%m%d-%H:%M:%S.000"); /* TODO: - ms */

    return os.str();
}
