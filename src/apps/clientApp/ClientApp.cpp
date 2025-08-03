/**
 * @file ClientApp.cpp
 * @author Edward Palmer
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ClientApp.hpp"
#include "Utilities/UUID.hpp"
#include <stdexcept>
#include <unistd.h>

/* TODO: - rewrite to make consistent with the server application */

ClientApp::ClientApp(int serverPort, std::size_t delayMS) : _delayMS(delayMS)
{
    if (!_fixClient.connectToServer(serverPort))
    {
        throw std::runtime_error("connection to server port " + std::to_string(serverPort) + " failed");
    }

    /* Setup dummy Fix */
    int clientSocket = _fixClient.clientSocket();

    _dummyFix.setTag(FixTag::MsgType, "8");
    _dummyFix.setTag(FixTag::Side, "1");
    _dummyFix.setTag(FixTag::Currency, "GBP");
    _dummyFix.setTag(FixTag::OrderQty, "1");
    _dummyFix.setTag(FixTag::Price, "100.00");
    _dummyFix.setTag(FixTag::ExecTransType, "0");
    _dummyFix.setTag(FixTag::ExecType, "0");
    _dummyFix.setTag(FixTag::Trace, "Client" + std::to_string(clientSocket));
}


void ClientApp::appLoop()
{
    while (!shouldStop())
    {
        _dummyFix.setTag(FixTag::ClOrdID, UUID::instance().generate(15)); /* New unique ID for client order */
        _fixClient.broadcast(_dummyFix);                                  /* Broadcast to the Order Management System Engine - NO: use FixServer implementation */

        std::this_thread::sleep_for(std::chrono::milliseconds(_delayMS));
    }
}
