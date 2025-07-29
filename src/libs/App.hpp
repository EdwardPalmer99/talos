/**
 * @file App.hpp
 * @author Edward Palmer
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include <atomic>
#include <thread>

class App
{
public:
    App();
    virtual ~App();

    /* No copy constructors */
    App(const App &) = delete;
    App &operator=(const App &) = delete;

    /* Non-blocking async execution */
    void start();

    void wait();

    void stop();

protected:
    /* Background thread loop triggered by start */
    virtual void appLoop() = 0;

    bool isRunning() const { return _isRunning; }

    bool shouldStop() const { return _shouldStop; }

private:
    std::thread _thread;
    std::atomic<bool> _isRunning{false};
    std::atomic<bool> _shouldStop{false};
};