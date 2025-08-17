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
    onRegisterNetAdminCmds();
}


void FixServer::onRegisterNetAdminCmds()
{
    /* Shutsdown server */
    registerNetAdminCmd("shutdown", [this](SocketFD socket)
    {
        sendNetAdminResponse("Commencing shutdown", socket);
        stop();
        wait();
    });

    /* Lists available commands */
    registerNetAdminCmd("list", [this](SocketFD socket)
    {
        std::ostringstream responseOS;

        {
            std::shared_lock guard(_netadminCmdsMutex);
            for (auto iter : _netadminCmds)
            {
                responseOS << iter.first << '\n'; /* TODO: - add usage */
            }
        }

        sendNetAdminResponse(responseOS.str(), socket);
    });

    /* TODO: - add additional commands to log statistics, performance, etc */
}


void FixServer::registerNetAdminCmd(std::string name, NetAdminCmdHandler handler)
{
    Logger::instance().debug("Registering netadmin command with name: " + name);

    std::unique_lock guard(_netadminCmdsMutex);
    _netadminCmds[name] = std::move(handler);
}


void FixServer::handleNetAdminCmd(FixMessage fixMsg, SocketFD netAdminSocket)
{
    if (!isNetAdminFixMessage(fixMsg))
    {
        Logger::instance().error("Received Fix is not of type NetAdmin => Ignoring.");
        return;
    }

    std::string adminCmd = fixMsg.getValue(FixTag::AdminCommand);

    NetAdminCmdMap::iterator iter;

    {
        std::shared_lock guard(_netadminCmdsMutex);
        iter = _netadminCmds.find(adminCmd);
        if (iter == _netadminCmds.end())
        {
            Logger::instance().error("Invalid admin command: " + adminCmd);
            return;
        }
    }

    Logger::instance().info("Calling netAdminCmd: " + adminCmd);
    iter->second(netAdminSocket);
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


bool FixServer::isNetAdminFixMessage(const FixMessage &fixMsg) const
{
    /* TODO: - stick into an enum for different message types */
    return (fixMsg.getValue(FixTag::MsgType) == "QR" && fixMsg.hasTag(FixTag::AdminCommand));
}
