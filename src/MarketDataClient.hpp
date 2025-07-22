/**
 * @file Client.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */


#pragma once
#include "Event.hpp"
#include "EventSubscriber.hpp"

/* TODO: - prior to start, we need to register all ISINs we want to register events for */

class MarketDataClient : public EventSubscriber
{
protected:
    void handleEvent(const Event &event) override;
};
