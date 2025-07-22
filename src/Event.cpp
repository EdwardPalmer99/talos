/**
 * @file Event.cpp
 * @author Edward Palmer
 * @date 2025-07-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Event.hpp"

Event::Event(std::string type, std::string id) : _type{type},
                                                 _id{id},
                                                 _timestamp(generateUTCTimestamp())
{
}

std::string Event::generateUTCTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    time_t now_t = std::chrono::system_clock::to_time_t(now); /* Convert */

    /* Extract microseconds */
    auto now_micro = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;

    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now_t), "%Y%m%d-%H:%M:%s") << "." << std::setfill('0') << std::setw(6) << now_micro.count();

    return oss.str();
}