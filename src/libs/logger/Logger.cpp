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
    _nameForLogLevel = {
        {Debug, "debug"},
        {Info, "info"},
        {Warn, "warn"},
        {Error, "error"},
        {Critical, "critical"}};
}


Logger::~Logger()
{
    stop();
    wait(); /* Ensures logger shutdown completed */
}


const std::string &Logger::logLevelName(Level level) const
{
    return _nameForLogLevel.at(level);
}


bool Logger::isLoggable(Level level)
{
    return (level >= _logLevel);
}


void Logger::setLevel(Level level)
{
    _logLevel = level;
}


void Logger::start()
{
    if (_running)
        return;

    _running = true;
    _loggerThread = std::thread(&Logger::loggerLoop, this);
}


void Logger::stop()
{
    if (!_running)
        return;

    _running = false;
    _loggerCV.notify_one(); /* Wake-up the logger thread */
}


void Logger::wait()
{
    if (_loggerThread.joinable())
        _loggerThread.join();
}


void Logger::log(std::string message, Level level)
{
    if (!isLoggable(level))
        return;

    /* Construct our output message */
    std::ostringstream os;
    os << nowUTC() << " " << logLevelName(level) << ": " << std::move(message);

    {
        std::unique_lock lock(_loggerMutex);
        _loggerQueue.push(os.str());
    } /* End of lock scope. Call notify after unlocking to avoid waking-up waiting thread only to block again */

    _loggerCV.notify_one();
}


void Logger::loggerLoop()
{
    while (true)
    {
        std::unique_lock lock(_loggerMutex);
        _loggerCV.wait(lock, [this]()
        {
            return (!_loggerQueue.empty() || !_running);
        });

        if (!_running) /* Terminate */
        {
            std::cout << std::flush; /* Flush anything remaining to stdout */
            return;
        }

        std::cout << _loggerQueue.front() << std::endl;
        _loggerQueue.pop();
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
