/**
 * @file MarketData.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "Event.hpp"
#include <string>


class MarketDataEvent : public Event
{
public:
    MarketDataEvent(std::string isin) : Event("MarketData", isin) {}

    double bid;
    double ask;

    int bidSize;
    int askSize;

    double lastSale;
    int lastSize;

    std::string quoteTime;
    std::string tradeTime;

    std::string exchange;

    int volume;
};
