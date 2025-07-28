/**
 * @file MarketData.hpp
 * @author Edward Palmer
 * @date 2025-07-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <ostream>
#include <string>


struct Event
{
    Event(std::string symbol, std::string currency, std::string date, double closePrice)
        : symbol(std::move(symbol)), currency(std::move(currency)), date(std::move(date)), closePrice(closePrice) {}

    std::string symbol;   /* XXX */
    std::string currency; /* USD, GBP */
    std::string date;     /* YYYY-MM-DD */
    double closePrice;    /* XXX.XX */
};
