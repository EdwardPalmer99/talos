/**
 * @file MovingAverage.hpp
 * @author Edward Palmer
 * @date 2025-07-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

class MovingAverage
{
public:
    MovingAverage() = delete;
    MovingAverage(unsigned int n = 10);

    ~MovingAverage();

    [[nodiscard]] inline bool ready() const;

    void update(double next);

    double average() const;

private:
    const unsigned int _n{0}; /* Size of circular buffer */

    unsigned int _writeIndex{0};

    bool _ready{false};

    double _runningTotal{0.0};
    double *_circularBuffer{nullptr};
};


bool MovingAverage::ready() const
{
    return _ready;
}