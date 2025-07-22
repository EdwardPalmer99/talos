/**
 * @file EventPublisher.cpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "EventPublisher.hpp"
#include "Event.hpp"
#include "EventLogger.hpp"
#include "EventSubscriber.hpp"
#include <stdexcept>


void EventPublisher::subscribe(EventSubscriber *subscriber, std::string type, std::string id)
{
    if (!subscriber || type.empty() || id.empty())
    {
        throw std::invalid_argument("null subscriber or empty event type/id");
    }

    LockGuard lock(_eventPublisherMutex);

    _notifySubscribersForEvent[std::make_pair(type, id)].insert(subscriber);
}


void EventPublisher::unsubscribe(EventSubscriber *subscriber, std::string type, std::string id)
{
    if (!subscriber || type.empty() || id.empty())
    {
        throw std::invalid_argument("null subscriber or empty event type/id");
    }

    LockGuard lock(_eventPublisherMutex);

    auto iter = _notifySubscribersForEvent.find(std::make_pair(type, id));
    if (iter == _notifySubscribersForEvent.end())
    {
        throw std::logic_error("no subscribers for event type/id");
    }

    iter->second.erase(subscriber);
}


void EventPublisher::publishEvent(const Event &event)
{
    // EventLogger::instance().log("Received event with id " + event.id());

    /* TODO: - add logging here. Need to create async logger on separate thread to use across different threads */
    auto iter = _notifySubscribersForEvent.find(std::pair(event.type(), event.id()));
    if (iter == _notifySubscribersForEvent.end())
    {
        EventLogger::instance().log("No subscribers for event with id " + event.id());
        return; /* No-one to notify */
    }

    for (EventSubscriber *subscriber : iter->second)
    {
        // EventLogger::instance().log("Publishing event with id " + event.id());
        subscriber->eventQueue().publish(event);
    }
}
