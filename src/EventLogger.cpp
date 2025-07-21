/**
 * @file EventLogger.cpp
 * @author Edward Palmer
 * @date 2025-07-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "EventLogger.hpp"
#include <iostream>


EventLogger &EventLogger::instance()
{
    static EventLogger instance;
    return instance;
}


void EventLogger::log(const std::string &message) const
{
    LockGuard lock(_eventLoggerMutex);
    std::cout << message << std::endl;
}