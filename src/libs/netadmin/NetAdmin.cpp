/**
 * @file NetAdmin.cpp
 * @author Edward Palmer
 * @date 2025-08-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "NetAdmin.hpp"
#include "fix/FixTag.hpp"
#include "utilities/UUID.hpp"


void NetAdmin::sendAdminCommand(std::string command)
{
    auto sockets = _portSocketMappings.getSockets();
    if (sockets.empty())
    {
        Logger::instance().error("No active sockets.");
        return;
    }

    auto adminFix = buildAdminCommand(command);

    for (auto socket : sockets)
    {
        sendFixMessage(adminFix, socket);
    }
}


FixMessage NetAdmin::buildAdminCommand(const std::string &command) const
{
    FixMessage fix;
    fix.setTag(FixTag::MsgType, "QR");
    fix.setTag(FixTag::ExecID, UUID::instance().generate());
    fix.setTag(FixTag::AdminCommand, command);

    return fix;
}


void NetAdmin::handleFixMessage(FixMessage message, SocketFD socket)
{
    std::string thePort = std::to_string(_portSocketMappings.getPort(socket));
    std::string theMsgType = message.getValue(FixTag::MsgType);

    if (message.getValue(FixTag::MsgType) == "QR") /* Received admin response */
    {
        Logger::instance().info("Received AdminResponse (sender: " + thePort + "):\n" + message.getValue(FixTag::AdminResponse));
        return;
    }

    Logger::instance().error("Received message with invalid msgType: " + theMsgType + ", from port: " + thePort);
}
