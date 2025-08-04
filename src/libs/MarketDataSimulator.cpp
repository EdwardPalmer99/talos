/**
 * @file MarketDataSimulator.cpp
 * @author Edward Palmer
 * @date 2025-07-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "MarketDataSimulator.hpp"
#include "MarketDataFeedHandler.hpp"
#include "events/Event.hpp"
#include "events/EventLogger.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>
#include <unistd.h>


MarketDataSimulator::MarketDataSimulator(std::string symbol, std::string currency, std::string csvPath, char delimiter)
{
    std::fstream csvStream(csvPath, std::ios_base::in);
    if (!csvStream.is_open())
    {
        throw std::runtime_error("failed to open marketdata CSV file with path: " + csvPath);
    }

    std::string line;
    while (std::getline(csvStream, line))
    {
        /* Split by delimiter in CSV file */
        std::size_t pos = line.find(delimiter);
        if (pos == std::string::npos)
        {
            throw std::logic_error("failed to find delimiter in CSV file");
        }

        std::string date = line.substr(0, pos);
        std::string closePrice = line.substr(pos + 1);

        Event event(symbol, currency, date, std::stod(closePrice, nullptr));
        _marketData.push(std::move(event));
    }

    /* Subscribe the feed handler to the symbol so it can receive updates */
    subscribe(&MarketDataFeedHandler::instance(), symbol);
}


void MarketDataSimulator::start()
{
    if (_isRunning)
        return;

    _isRunning = true;
    _eventLoopThread = std::thread(&MarketDataSimulator::loop, this);
}


void MarketDataSimulator::loop()
{
    while (!_marketData.empty())
    {
        publishEvent(_marketData.front());
        _marketData.pop();

        sleep(0.1);
    }
}