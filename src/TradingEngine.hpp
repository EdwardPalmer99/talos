/**
 * @file TradingEngine.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */


#pragma once
#include "Event.hpp"
#include "EventSubscriber.hpp"
#include "MovingAverage.hpp"
#include <string>


class TradingEngine : public EventSubscriber
{
public:
    TradingEngine() = delete;
    TradingEngine(double funds, std::string symbol);

    [[nodiscard]] inline double funds() const;

    [[nodiscard]] inline int shares() const;

protected:
    /* Execute strategy when we receive an event */
    void handleEvent(const Event &event) override;

    void logEvent(const Event &event) const;

private:
    double _funds{0.0};
    const double _originalFunds{0.0};

    int _units{0};

    MovingAverage _averagePrice50{5};
    MovingAverage _averagePrice200{20};
};


double TradingEngine::funds() const
{
    return _funds;
}


int TradingEngine::shares() const
{
    return _units;
}