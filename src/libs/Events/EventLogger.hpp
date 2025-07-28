/**
 * @file EventLogger.hpp
 * @author Edward Palmer
 * @date 2025-07-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class Logger
{
public:
    enum Level
    {
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3,
        Critical = 4
    };

    static Logger &instance();

    void log(std::string message, Level level = Info);

protected:
    Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    void loop();

    std::string levelToString(Level level) const;

    std::string nowUTC() const;

private:
    using LockGuard = std::lock_guard<std::mutex>;

    mutable std::mutex _loggerMutex;
    std::thread _loggerThread; /* Logger thread for writing messages to log */
    std::condition_variable _conditionVariable;

    std::queue<std::string> _loggerQueue; /* Log messages to write to out */
};