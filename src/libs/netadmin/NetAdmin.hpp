/**
 * @file NetAdmin.hpp
 * @author Edward Palmer
 * @date 2025-08-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "fix/FixMessage.hpp"
#include "socket/FixClient.hpp"
#include <condition_variable>
#include <mutex>
#include <string>

/* Connect to a server and send admin commands */
class NetAdmin : public FixClient
{
public:
    NetAdmin();

    /* Send an admin command to all connections */
    void sendAdminCommand(std::string command, std::size_t timeoutSeconds = 2);

protected:
    void handleFixMessage(FixMessage message, SocketFD socket) final;

    void onShutdown() override;

    FixMessage buildAdminCommand(const std::string &command) const;

private:
    bool _hasResponse{false};
    std::mutex _responseMutex;
    std::condition_variable _responseCV;
};