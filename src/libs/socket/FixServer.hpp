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
    using NetAdminCmdHandler = std::function<void(SocketFD)>;
    using MsgTypeHandler = std::function<void(FixMessage, SocketFD)>;

    FixServer(Port port) : FixEndpoint<Server>(port) {}

    void registerMsgTypeHandler(std::string msgType, MsgTypeHandler handler);
    void registerNetAdminCmdHandler(std::string cmd, NetAdminCmdHandler handler);

    void sendNetAdminResponse(std::string response, SocketFD netAdminSocket);

    /* Hooks */
    virtual void onRegisterMsgTypes();
    virtual void onRegisterNetAdminCmds();

private:
    /* Adds hooks */
    void onStartup() final;

    /* Maps message to registered handler */
    void handleFixMessage(FixMessage message, SocketFD socket) final;

    using NetAdminCmdMap = std::unordered_map<std::string, NetAdminCmdHandler>;
    using MsgTypeHandlerMap = std::unordered_map<std::string, MsgTypeHandler>;

    NetAdminCmdMap _handlerForNetAdminCmd;
    std::shared_mutex _netadminCmdsMutex;

    MsgTypeHandlerMap _handlerForMsgType;
    std::shared_mutex _handlerForMsgTypeMutex;
};
