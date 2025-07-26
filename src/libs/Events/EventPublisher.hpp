/**
 * @file EventPublisher.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "EventSubscriber.hpp"
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>


class Event;


class EventPublisher
{
public:
    EventPublisher() = default;

    virtual ~EventPublisher() = default;

    /* Subscribes to a particular symbol */
    void subscribe(EventSubscriber *subscriber, std::string symbol);

    /* Unsubscribes from a particular symbol */
    void unsubscribe(EventSubscriber *subscriber, std::string symbol);

protected:
    /* Publish an event to all subscribers */
    void publishEvent(const Event &event);

private:
    // TODO: - slightly pointless using a map since we only deal with a single symbol
    /* Maps from symbol --> subscribers */
    std::unordered_map<std::string, std::unordered_set<EventSubscriber *>> _notifySubscribersForEvent;

    mutable std::mutex _eventPublisherMutex;
    using LockGuard = std::lock_guard<std::mutex>;
};
