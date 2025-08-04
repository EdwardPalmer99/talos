/**
 * @file App.cpp
 * @author Edward Palmer
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "App.hpp"
#include "logger/Logger.hpp"


App::App()
{
    Logger::instance().log("starting App...");
}


App::~App()
{
    Logger::instance().log("shutting-down App...");

    stop();
    wait();                        /* Ensure thread completes before destructing app */
    Logger::instance().shutdown(); /* Shutdown logger */
}


void App::start()
{
    if (_isRunning)
    {
        return;
    }

    _thread = std::thread(&App::appLoop, this);
    _shouldStop = false; /* Reset */
    _isRunning = true;
}


void App::wait()
{
    if (_thread.joinable())
    {
        _thread.join();
    }
}


void App::stop()
{
    if (_shouldStop)
    {
        return;
    }

    _shouldStop = true;
}