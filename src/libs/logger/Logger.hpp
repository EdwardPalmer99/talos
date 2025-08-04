/**
 * @file Logger.hpp
 * @author Edward Palmer
 * @date 2025-07-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>


class Logger
{
public:
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    static Logger &instance();

    void start();

    void stop();

    void wait();

    enum Level
    {
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3,
        Critical = 4
    };

    void log(std::string message, Level level = Info);

    void setLevel(Level level);

    inline void debug(std::string message);
    inline void info(std::string message);
    inline void warn(std::string message);
    inline void error(std::string message);
    inline void critical(std::string message);

protected:
    Logger();
    ~Logger();

    bool isLoggable(Level level);

    const std::string &logLevelName(Level level) const;

    std::string nowUTC() const;

    void loggerLoop();

private:
    mutable std::mutex _loggerMutex;
    std::thread _loggerThread;
    std::condition_variable _loggerCV;
    std::queue<std::string> _loggerQueue;
    std::unordered_map<Level, std::string> _nameForLogLevel;

    std::atomic<bool> _running{false};
    std::atomic<Level> _logLevel{Level::Info};
};


void Logger::debug(std::string message)
{
    log(std::move(message), Debug);
}


void Logger::info(std::string message)
{
    log(std::move(message), Info);
}


void Logger::warn(std::string message)
{
    log(std::move(message), Warn);
}


void Logger::error(std::string message)
{
    log(std::move(message), Error);
}


void Logger::critical(std::string message)
{
    log(std::move(message), Critical);
}