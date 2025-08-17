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
#include "logger/Logger.hpp"
#include "utilities/UUID.hpp"
#include <iostream>

NetAdmin::NetAdmin()
{
    Logger::instance().setLevel(Logger::Warn); /* Only print this or above to the iostream */
}


void NetAdmin::sendAdminCommand(std::string command, std::size_t timeoutSeconds)
{
    auto sockets = _portSocketMappings.getSockets();
    if (sockets.empty())
    {
        std::cerr << "No active sockets" << std::endl;
        return;
    }

    auto adminFix = buildAdminCommand(command);
    for (auto socket : sockets)
    {
        sendFixMessage(adminFix, socket);
    }

    /* wait timeoutSeconds or until we get a response */
    std::unique_lock lock(_responseMutex);
    _responseCV.wait_for(lock, std::chrono::seconds(timeoutSeconds), [this]
    { return _hasResponse; });
}


void NetAdmin::onShutdown()
{
    std::unique_lock lock(_responseMutex);
    if (!_hasResponse)
    {
        std::cerr << "Timed-out waiting for a response." << std::endl;
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
        std::cout << message.getValue(FixTag::AdminResponse) << std::endl;
    }
    else
    {
        std::cerr << "Received an invalid response." << std::endl;
    }

    {
        std::unique_lock lock(_responseMutex);
        _hasResponse = true;
    } /* Slight limitation is that if we are sending to multiple sockets we return true for first response only */

    _responseCV.notify_all(); /* Notify blocking sendAdminCommand */
}
