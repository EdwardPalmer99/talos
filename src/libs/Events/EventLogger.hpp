/**
 * @file EventLogger.hpp
 * @author Edward Palmer
 * @date 2025-07-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <mutex>
#include <string>


class EventLogger
{
public:
    static EventLogger &instance();

    void log(const std::string &message) const;

protected:
    EventLogger() = default;
    EventLogger(const EventLogger &) = delete;
    EventLogger &operator=(const EventLogger &) = delete;

private:
    using LockGuard = std::lock_guard<std::mutex>;
    mutable std::mutex _eventLoggerMutex;
};