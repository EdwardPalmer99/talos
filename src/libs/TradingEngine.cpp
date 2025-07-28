/**
 * @file TradingEngine.cpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "TradingEngine.hpp"
#include "Events/Event.hpp"
#include "Events/EventLogger.hpp"
#include "MarketDataFeedHandler.hpp"
#include <iostream>
#include <sstream>

TradingEngine::TradingEngine(double funds, std::string symbol)
    : _funds(funds), _originalFunds(funds)
{
    /* Subscribe to receive events for the symbol we want to trade */
    MarketDataFeedHandler::instance().subscribe(this, symbol);
}


void TradingEngine::handleEvent(const Event &event)
{
    if (_averagePrice200.ready() && _averagePrice50.average() > _averagePrice200.average())
    {
        int dUnits = std::min(50, (int)(event.closePrice / _funds));

        std::cout << "IN THIS BIT" << std::endl;
        _units += dUnits;
        _funds -= 50.0 * event.closePrice;
    }
    else if (_averagePrice200.ready() && _averagePrice50.average() < _averagePrice200.average())
    {
        int dUnits = std::min(50, _units);

        _units -= dUnits;
        _funds += dUnits * event.closePrice;

        std::cout << "IN THIS OTHER BIT" << std::endl;
    }

    if (_averagePrice200.ready())
    {
        std::cout << _averagePrice200.average() << ", " << _averagePrice50.average() << std::endl;
    }

    logEvent(event);
}


void TradingEngine::logEvent(const Event &event) const
{
    std::ostringstream output;

    double pnl = _funds + _units * event.closePrice - _originalFunds;

    output << "[" << "funds: " << _funds << "; shares: " << _units << "; total: " << _funds + _units * event.closePrice << "; PnL: " << pnl << "]";
    Logger::instance().log(output.str());
}
