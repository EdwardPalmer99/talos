/**
 * @file FixServer.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "FixEndpoint.hpp"
#include "Server.hpp"
#include <fix/FixMessage.hpp>
#include <functional>
#include <shared_mutex>
#include <string>
#include <unordered_map>


class FixServer : public FixEndpoint<Server>
{
protected:
    FixServer(Port port) : FixEndpoint<Server>(port) {}

    using NetAdminCmdHandler = std::function<void(SocketFD)>;

    bool isNetAdminFixMessage(const FixMessage &fixMsg) const;

    void registerNetAdminCmd(std::string cmd, NetAdminCmdHandler handler);

    void sendNetAdminResponse(std::string response, SocketFD netAdminSocket);

    /* Hook to override to add additional netadmin commands */
    virtual void onRegisterNetAdminCmds();

    /* Add netadmin hook into onStartup() */
    virtual void onStartup() override;

    void handleNetAdminCmd(FixMessage fixMsg, SocketFD netAdminSocket);

private:
    using NetAdminCmdMap = std::unordered_map<std::string, NetAdminCmdHandler>;

    NetAdminCmdMap _netadminCmds;
    std::shared_mutex _netadminCmdsMutex;
};
