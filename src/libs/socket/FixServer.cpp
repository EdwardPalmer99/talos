/**
 * @file FixServer.cpp
 * @author Edward Palmer
 * @date 2025-08-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "socket/FixServer.hpp"
#include <sstream>


void FixServer::onStartup()
{
    FixEndpoint<Server>::onStartup();
    onRegisterMsgTypes();
    onRegisterNetAdminCmds();
}


void FixServer::onRegisterMsgTypes()
{
    /* TODO: - other servers should override this to add their own registered types */
    auto handler = [this](FixMessage fixMsg, SocketFD netAdminSocket) -> void
    {
        std::string adminCmd = fixMsg.getValue(FixTag::AdminCommand);

        NetAdminCmdMap::iterator iter;

        {
            std::shared_lock guard(_netadminCmdsMutex);
            iter = _handlerForNetAdminCmd.find(adminCmd);
            if (iter == _handlerForNetAdminCmd.end())
            {
                Logger::instance().error("Ignoring unregistered command: " + adminCmd);
                return;
            }
        }

        iter->second(netAdminSocket);
    };

    registerMsgTypeHandler("QR", std::move(handler));
}


void FixServer::registerMsgTypeHandler(std::string msgType, MsgTypeHandler handler)
{
    Logger::instance().debug("Registering MsgType " + msgType);

    std::unique_lock guard(_handlerForMsgTypeMutex);
    _handlerForMsgType[msgType] = std::move(handler);
}


void FixServer::onRegisterNetAdminCmds()
{
    /* Shutsdown server */
    registerNetAdminCmdHandler("shutdown", [this](SocketFD socket)
    {
        sendNetAdminResponse("Commencing shutdown", socket);
        stop();
        wait();
    });

    /* Lists available commands */
    registerNetAdminCmdHandler("list", [this](SocketFD socket)
    {
        std::ostringstream responseOS;

        {
            std::shared_lock guard(_netadminCmdsMutex);
            for (auto iter : _handlerForNetAdminCmd)
            {
                responseOS << iter.first << '\n'; /* TODO: - add usage */
            }
        }

        sendNetAdminResponse(responseOS.str(), socket);
    });

    /* TODO: - add additional commands to log statistics, performance, etc */
}


void FixServer::registerNetAdminCmdHandler(std::string cmd, NetAdminCmdHandler handler)
{
    Logger::instance().debug("Registering netadmin command with cmd: " + cmd);

    std::unique_lock guard(_netadminCmdsMutex);
    _handlerForNetAdminCmd[cmd] = std::move(handler);
}


void FixServer::sendNetAdminResponse(std::string response, SocketFD netAdminSocket)
{
    if (response.empty() || netAdminSocket == (-1))
    {
        Logger::instance().error("Invalid response/destination => Ignoring.");
    }

    FixMessage responseFix;
    responseFix.setTag(FixTag::MsgType, "QR");
    responseFix.setTag(FixTag::AdminResponse, std::move(response));

    sendFixMessage(std::move(responseFix), netAdminSocket);
}


void FixServer::handleFixMessage(FixMessage message, SocketFD socket)
{
    std::string msgType(message.getValue(FixTag::MsgType));

    MsgTypeHandlerMap::iterator iter;

    {
        std::shared_lock guard(_handlerForMsgTypeMutex);
        iter = _handlerForMsgType.find(msgType);
        if (iter == _handlerForMsgType.end())
        {
            Logger::instance().error("No handler registered for msgType " + msgType);
            return;
        }
    }

    iter->second(std::move(message), socket); /* Call */
}
