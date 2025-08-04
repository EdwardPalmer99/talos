/**
 * @file MarketDataFeedHandler.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "TradingEngine.hpp"
#include "events/EventHandler.hpp"
#include "events/EventPublisher.hpp"
#include "events/EventSubscriber.hpp"
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

class Event;

/**
 * MarketDataFeed(s)... --> MarketDataFeedHandler --> MarketDataClient(s)...
 */
class MarketDataFeedHandler : public EventSubscriber, public EventPublisher
{
public:
    static MarketDataFeedHandler &instance();

protected:
    MarketDataFeedHandler() = default;
    MarketDataFeedHandler(const MarketDataFeedHandler &) = delete;
    MarketDataFeedHandler &operator=(const MarketDataFeedHandler &) = delete;

    /* Handle incoming events we receive from the market data feeds */
    void handleEvent(const Event &event) override;
};
