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
#include <string>

/* Connect to a server and send admin commands */
class NetAdmin : public FixClient
{
public:
    /* Send an admin command to all connections */
    void sendAdminCommand(std::string command);

protected:
    void handleFixMessage(FixMessage message, SocketFD socket) final;

    FixMessage buildAdminCommand(const std::string &command) const;
};