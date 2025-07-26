/**
 * @file EventHandler.cpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "EventHandler.hpp"
#include "Event.hpp"
#include "EventLogger.hpp"


void EventHandler::start()
{
    if (_isRunning)
        return;

    _isRunning = true;
    _eventLoopThread = std::thread(&EventHandler::loop, this);
}


void EventHandler::notify()
{
    _conditionVariable.notify_one();
}


void EventHandler::loop()
{
    while (true)
    {
        Lock lock(_eventHandlerMutex);
        _conditionVariable.wait(lock, [this]()
        {
            return eventQueue().hasData();
        });

        handleEvent(eventQueue().getData());
    }
}
