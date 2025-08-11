/**
 * @file FixEndpoint.hpp
 * @author Edward Palmer
 * @date 2025-08-11
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "fix/FixMessage.hpp"
#include "fix/FixTag.hpp"
#include "logger/Logger.hpp"
#include "socket/ConnectionManager.hpp"
#include <chrono>
#include <iomanip>
#include <string>

// TODO: - also set message sequence #


template <typename Transport>
class FixEndpoint : public Transport
{
public:
    using Transport::Transport;

protected:
    virtual void handleFixMessage(FixMessage message, ConnectionManager::SocketFD serverSocket) = 0;

    virtual void enrichFixMessage(FixMessage &message);

    void sendFixMessage(FixMessage message, ConnectionManager::SocketFD socket)
    {
        enrichFixMessage(message);
        Logger::instance().debug("Sent FixMsg (destination: " + std::to_string(socket) + "): " + message.toString());
        Transport::sendMessage(message.toString(), socket);
    }

    std::string nowUTC() const;

private:
    void handleMessage(std::string message, ConnectionManager::SocketFD socket) final
    {
        Logger::instance().debug("Received FixMsg (source: " + std::to_string(socket) + "): " + message);
        handleFixMessage(FixMessage(std::move(message)), socket);
    }
};


template <typename Transport>
void FixEndpoint<Transport>::enrichFixMessage(FixMessage &message)
{
    message.setTag(FixTag::SendingTime, nowUTC());
}


template <typename Transport>
std::string FixEndpoint<Transport>::nowUTC() const
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::ostringstream os;

    struct tm currentGMTime; /* gmtime_r is a thread-safe verison */
    os << std::put_time(gmtime_r(&currentTime, &currentGMTime), "%Y%m%d-%H:%M:%S") << "." << std::setw(3) << std::setfill('0') << ms;

    return os.str();
}
