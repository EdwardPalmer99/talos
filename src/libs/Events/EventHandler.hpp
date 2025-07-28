/**
 * @file EventHandler.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "EventQueue.hpp"
#include <condition_variable>
#include <mutex>
#include <thread>

class Event;


class EventHandler
{
public:
    EventHandler() : _eventQueue(this) {}

    virtual ~EventHandler() = default;

    /* Creates a new thread to run event loop */
    void start();

    /* Returns a reference to a client's event queue */
    [[nodiscard]] inline EventQueue &eventQueue();

    [[nodiscard]] inline bool isRunning() const;

    [[nodiscard]] inline std::thread &thread();

protected:
    friend class EventQueue;

    /* Start event loop. Polls waiting for event data and calls handleEvent() */
    void loop();

    /* Notify that a new event has been published to the queue */
    void notify();

    virtual void handleEvent(const Event &event) = 0;

private:
    using Lock = std::unique_lock<std::mutex>;

    mutable std::mutex _eventHandlerMutex;
    std::condition_variable _conditionVariable;

    EventQueue _eventQueue;

    bool _isRunning{false};
    std::thread _eventLoopThread;
};

/* TODO: - properly handle shutdown with ctrl-c */


EventQueue &EventHandler::eventQueue()
{
    return _eventQueue;
}


bool EventHandler::isRunning() const
{
    return _isRunning;
}


std::thread &EventHandler::thread()
{
    return _eventLoopThread;
}
