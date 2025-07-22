/**
 * @file MarketDataFeed.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "EventPublisher.hpp"


class Event;


/* TODO: - create a way to read CSV files with historical marketdata and publish at correct times */


class MarketDataFeed : public EventPublisher
{
public:
    using EventPublisher::publishEvent; /* Make public for testing only */

    /* Add methods to generate synthetic marketdata events @ random intervals */
};
