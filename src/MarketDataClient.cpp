/**
 * @file MarketDataClient.cpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "MarketDataClient.hpp"
#include "Event.hpp"
#include "EventLogger.hpp"
#include "MarketDataEvent.hpp"
#include <iostream>


void MarketDataClient::handleEvent(const Event &event)
{
    EventLogger::instance().log("MarketDataClient Received an event with id " + event.id());

    auto *marketDataEvent = dynamic_cast<const MarketDataEvent *>(&event);
    if (!marketDataEvent)
    {
        return; /* Ignore other type of event */
    }

    /* Do something based on this. i.e. execute a trading strategy */

    /* Log the event we received here */
}
