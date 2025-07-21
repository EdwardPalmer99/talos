/**
 * @file MarketDataFeedHandler.cpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "MarketDataFeedHandler.hpp"
#include "Event.hpp"
#include "EventLogger.hpp"
#include "MarketDataEvent.hpp"
#include <stdexcept>


MarketDataFeedHandler &MarketDataFeedHandler::instance()
{
    static MarketDataFeedHandler instance;
    return instance;
}


void MarketDataFeedHandler::handleEvent(const Event &event)
{
    EventLogger::instance().log("MarketDataFeedHandler received event with id " + event.id());
    publishEvent(event); /* Forward received event onto clients */
}