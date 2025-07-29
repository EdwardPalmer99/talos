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

    /* Thread-safe logging */
    void log(std::string message, Level level = Info);

    /* Thread-safe call to shutdown the logger loop. To be called by applications in their destructors */
    void shutdown();

protected:
    Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    ~Logger();

    void loop();

    std::string levelToString(Level level) const;

    std::string nowUTC() const;

private:
    mutable std::mutex _loggerMutex;
    std::thread _loggerThread; /* Logger thread for writing messages to log */
    std::condition_variable _conditionVariable;

    std::queue<std::string> _loggerQueue; /* Log messages to write to out */

    bool _running{false};
};