/**
 * @file MarketDataFeedHandler.cpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "MarketDataFeedHandler.hpp"
#include "events/Event.hpp"
#include "events/EventLogger.hpp"
#include <stdexcept>


MarketDataFeedHandler &MarketDataFeedHandler::instance()
{
    static MarketDataFeedHandler instance;
    return instance;
}


void MarketDataFeedHandler::handleEvent(const Event &event)
{
    publishEvent(event); /* Forward received event onto clients */
}