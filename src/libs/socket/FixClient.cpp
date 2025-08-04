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
    /* Set additional required tags */
    message.setTag(FixTag::MsgSeqNo, std::to_string(_msgSeqNo++));
    message.setTag(FixTag::SendingTime, sendingTimeUTC());

    /* Delegate to base class */
    return Client::broadcast(message.toString());
}


std::string FixClient::sendingTimeUTC() const
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::ostringstream os;

    struct tm currentGMTime; /* gmtime_r is a thread-safe verison */
    os << std::put_time(gmtime_r(&currentTime, &currentGMTime), "%Y%m%d-%H:%M:%S") << "." << std::setw(3) << std::setfill('0') << ms;

    return os.str();
}
