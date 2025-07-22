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

    // TODO: - use 'id = *' to subscribe to all events of that type
    /* Subscribes to a particular event type */
    void subscribe(EventSubscriber *subscriber, std::string type, std::string id);

    /* Unsubscribes from a particular event type */
    void unsubscribe(EventSubscriber *subscriber, std::string type, std::string id);

protected:
    /* Publish an event to all subscribers */
    void publishEvent(const Event &event);

private:
    using EventPair = std::pair<std::string, std::string>; /* (type, id) */

    struct EventPairHash
    {
        std::size_t operator()(const EventPair &pair) const
        {
            auto h1 = std::hash<std::string>{}(pair.first);
            auto h2 = std::hash<std::string>{}(pair.second);

            return (h1 ^ h2);
        }
    };

    std::unordered_map<EventPair, std::unordered_set<EventSubscriber *>, EventPairHash> _notifySubscribersForEvent;

    mutable std::mutex _eventPublisherMutex;
    using LockGuard = std::lock_guard<std::mutex>;
};
