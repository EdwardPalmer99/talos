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


void EventPublisher::subscribe(EventSubscriber *subscriber, std::string symbol)
{
    if (!subscriber || symbol.empty())
    {
        throw std::invalid_argument("null subscriber or empty symbol");
    }

    LockGuard lock(_eventPublisherMutex);

    _notifySubscribersForEvent[symbol].insert(subscriber);
}


void EventPublisher::unsubscribe(EventSubscriber *subscriber, std::string symbol)
{
    if (!subscriber || symbol.empty())
    {
        throw std::invalid_argument("null subscriber or empty event symbol");
    }

    LockGuard lock(_eventPublisherMutex);

    auto iter = _notifySubscribersForEvent.find(symbol);
    if (iter == _notifySubscribersForEvent.end())
    {
        throw std::logic_error("no subscribers for symbol");
    }

    iter->second.erase(subscriber);
}


void EventPublisher::publishEvent(const Event &event)
{
    LockGuard lock(_eventPublisherMutex);

    auto iter = _notifySubscribersForEvent.find(event.symbol);
    if (iter == _notifySubscribersForEvent.end())
    {
        return; /* No-one to notify */
    }

    for (EventSubscriber *subscriber : iter->second)
    {
        subscriber->eventQueue().publish(event);
    }
}
