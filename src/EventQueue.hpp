/**
 * @file EventQueue.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */


#pragma once
#include "Event.hpp"
#include <mutex>
#include <queue>
#include <stdexcept>

class EventHandler;

/* TODO: - would be more efficient to use a read-write lock */

/*
 * Clients will each have their own event-queue. They can poll in a loop for new data and on new data can call their
 * handle-event method.
 */
class EventQueue
{
public:
    EventQueue(EventHandler *handler);

    /* Thread-safe method for a publisher to publish an event to a queue */
    void publish(Event event);

    [[nodiscard]] inline bool hasData() const;

    /* Returns and pops the next Event in the queue */
    Event getData();

    /* Prevent copying of the queue for the moment */
    EventQueue(const EventQueue &) = delete;
    EventQueue &operator=(const EventQueue &) = delete;

private:
    using LockGuard = std::lock_guard<std::mutex>;

    std::queue<Event> _eventQueue;
    mutable std::mutex _eventQueueMutex;

    EventHandler *_eventHandler;
};


bool EventQueue::hasData() const
{
    LockGuard lock(_eventQueueMutex);
    return !_eventQueue.empty();
}