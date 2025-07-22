/**
 * @file EventQueue.cpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "EventQueue.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"
#include <iostream>

EventQueue::EventQueue(EventHandler *handler) : _eventHandler(handler)
{
}


void EventQueue::publish(Event event)
{
    LockGuard lock(_eventQueueMutex);
    _eventQueue.push(std::move(event));
    _eventHandler->notify();
}


Event EventQueue::getData()
{
    std::lock_guard lock(_eventQueueMutex);

    if (_eventQueue.empty())
    {
        throw std::out_of_range("event queue is empty");
    }

    auto firstEvent = _eventQueue.front();
    _eventQueue.pop();

    return firstEvent;
}