/**
 * @file Event.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

/* TODO: - add an event logger */
/* TODO: - our event queues should discard older data for a marketID if more available */

class Event
{
public:
    /* Unique event type and ID. Enables us to setup subscribing to particular type and id */
    Event(std::string type, std::string id);

    virtual ~Event() = default;

    [[nodiscard]] inline const std::string &type() const;

    [[nodiscard]] inline const std::string &id() const;

    [[nodiscard]] inline const std::string &timestamp() const;

protected:
    Event() = default;

    std::string generateUTCTimestamp() const;

private:
    const std::string _type;
    const std::string _id;
    const std::string _timestamp;
};


const std::string &Event::type() const
{
    return _type;
}

const std::string &Event::id() const
{
    return _id;
}


const std::string &Event::timestamp() const
{
    return _timestamp;
}
