/**
 * @file MovingAverage.cpp
 * @author Edward Palmer
 * @date 2025-07-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "MovingAverage.hpp"
#include <new>
#include <stdexcept>


MovingAverage::MovingAverage(unsigned int n)
    : _n(n), _circularBuffer(new double[n])
{
}


MovingAverage::~MovingAverage()
{
    if (!_circularBuffer)
        return;

    delete[] _circularBuffer;
}


void MovingAverage::update(double next)
{
    if (_ready)
    {
        _runningTotal -= _circularBuffer[_writeIndex];
    }

    _runningTotal += next;
    _circularBuffer[_writeIndex] = next;

    _writeIndex = ((_writeIndex + 1) % _n);

    if (!_ready && _writeIndex == 0)
    {
        _ready = true;
    }

    /* Update indices */

    /* TODO: - simplify events with just a price (since buy/sell similar). Ignore qty since probably large enough */
} /* Use historical trading data (can use STOCKS function in Numbers to generate historical data) */
/* Use socket programmng to separate marketdata app from trading engine */
/* Write a parser to convert compressed marketdata to smething usable */

double MovingAverage::average() const
{
    if (!ready())
    {
        throw std::logic_error("cannot compute moving average -- insufficent values");
    }

    return (_runningTotal / (double)_n);
}
