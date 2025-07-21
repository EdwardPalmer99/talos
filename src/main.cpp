/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "MarketDataClient.hpp"
#include "MarketDataEvent.hpp"
#include "MarketDataFeed.hpp"
#include "MarketDataFeedHandler.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    /* EXTERNAL TO OUR TRADING INFRASTRUCTURE */
    MarketDataFeed feed1;

    /* Subscribe the feed handler to each feed. TODO: - enable all MD symbols */
    feed1.subscribe(&MarketDataFeedHandler::instance(), "MarketData", "ABC");

    /* INTERNAL */

    /* Subscribe trading engines to the feed handler for interested symbols */
    MarketDataClient tradingEngine1;
    MarketDataFeedHandler::instance().subscribe(&tradingEngine1, "MarketData", "ABC");

    MarketDataClient tradingEngine2;
    MarketDataFeedHandler::instance().subscribe(&tradingEngine2, "MarketData", "ABC");

    /* Fire-up. We'll leave the feeds on the main-thread for now */
    tradingEngine1.start();
    tradingEngine2.start();
    MarketDataFeedHandler::instance().start();

    /* On main-thread we publish market data for a trade */
    // feed1.publishEvent(theEvent);

    /* Expect the feed handler to receive this and publish to the clients */
    while (true)
    {
        std::string isin;
        std::getline(std::cin, isin);

        MarketDataEvent theEvent(isin);
        feed1.publishEvent(theEvent);
    }

    tradingEngine1.thread().join();
    tradingEngine2.thread().join();
    MarketDataFeedHandler::instance().thread().join();

    return 0;
}