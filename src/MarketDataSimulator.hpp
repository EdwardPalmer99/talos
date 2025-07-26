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
#include <queue>
#include <string>
#include <thread>

class Event;


class MarketDataSimulator : public EventPublisher
{
public:
    MarketDataSimulator() = delete;
    MarketDataSimulator(std::string symbol, std::string currency, std::string pathToCSV, char delimiter = ',');

    /* Creates a thread and starts the marketdata simulator */
    void start();

    [[nodiscard]] inline bool isRunning() const;

    [[nodiscard]] inline std::thread &thread();

protected:
    /* Event loop */
    void loop();

private:
    bool _isRunning{false};
    std::thread _eventLoopThread;
    std::queue<Event> _marketData;
};


bool MarketDataSimulator::isRunning() const
{
    return _isRunning;
}

std::thread &MarketDataSimulator::thread()
{
    return _eventLoopThread;
}