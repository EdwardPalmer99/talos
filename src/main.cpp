/**
 * @file main.cpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "MarketDataFeedHandler.hpp"
#include "MarketDataSimulator.hpp"
#include "TradingEngine.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    /* EXTERNAL TO OUR TRADING INFRASTRUCTURE */
    /* TODO: - do not use absolute path as will break elsewhere */
    MarketDataSimulator simulator("0P0000WUU0.L", "GBP", "/home/ubuntu/feedhandler/data/SampleMarketData.csv"); /* Publishes updates to internal feed handler */

    /* INTERNAL */
    TradingEngine engine1(3000.00, "0P0000WUU0.L"); /* Trading engine receives updates from our internal feed-handler */

    MarketDataFeedHandler::instance().start(); /* Listen for updates */
    engine1.start();
    simulator.start(); /* Start generating synthetic data */

    /***********************/
    MarketDataFeedHandler::instance().thread().join();
    engine1.thread().join();
    simulator.thread().join();

    return 0;
}