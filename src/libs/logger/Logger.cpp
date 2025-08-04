/**
 * @file Logger.cpp
 * @author Edward Palmer
 * @date 2025-07-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Logger.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>


Logger &Logger::instance()
{
    static Logger instance;
    return instance;
}


Logger::Logger()
{
    _running = true;
    _loggerThread = std::thread(&Logger::loop, this);
}


Logger::~Logger()
{
    shutdown();
}


std::string Logger::levelToString(Level level) const
{
    switch (level)
    {
        case Debug:
            return "debug";
        case Info:
            return "info";
        case Warn:
            return "warn";
        case Error:
            return "error";
        case Critical:
            return "critical";
        default:
            return "";
    }
}


std::string Logger::nowUTC() const
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::ostringstream os;

    struct tm currentGMTime; /* gmtime_r is a thread-safe verison */
    os << std::put_time(gmtime_r(&currentTime, &currentGMTime), "%Y%m%d-%H:%M:%S");

    return os.str();
}


void Logger::log(std::string message, Level level)
{
    /* Construct our output message */
    std::ostringstream os;
    os << nowUTC() << " " << levelToString(level) << ": " << std::move(message);

    {
        std::unique_lock lock(_loggerMutex);
        _loggerQueue.push(os.str());
    } /* End of lock scope. Call notify after unlocking to avoid waking-up waiting thread only to block again */

    _conditionVariable.notify_one();
}


void Logger::setLogLevel(Level logLevel)
{
    {
        std::unique_lock lock(_loggerMutex);
        _logLevel = logLevel;
    }

    _conditionVariable.notify_one();
}


void Logger::loop()
{
    while (true)
    {
        std::unique_lock lock(_loggerMutex);
        _conditionVariable.wait(lock, [this]()
        {
            return (!_loggerQueue.empty() || !_running);
        });

        if (!_running) /* Terminate */
        {
            return;
        }

        std::cout << _loggerQueue.front() << std::endl
                  << std::flush;

        _loggerQueue.pop();
    }
}


void Logger::shutdown()
{
    if (!_running)
    {
        return; /* Already shutdown */
    }

    {
        std::unique_lock lock(_loggerMutex);
        _running = false;
    } /* End of lock scope */

    _conditionVariable.notify_one();

    if (_loggerThread.joinable())
    {
        _loggerThread.join(); /* Prevent continuing until loop() terminates */
    }
}
