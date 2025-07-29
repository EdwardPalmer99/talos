/**
 * @file TraderApp.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Events/EventLogger.hpp"
#include "Fix/FixTag.hpp"
#include "Socket/ConnectionPorts.hpp"
#include "Socket/FixClient.hpp"
#include "Utilities/UUID.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

/**
 * Front-office application for traders to book, amend, cancel trades
 *
 * ClientApp (new, cancel, correct) ---> OrderManagementSystem
 *
 */
int main(void)
{
    Logger::instance().log("Starting FOBooking application...");

    /* Unique trader ID we assign to this application */
    const std::string kTraderID{UUID::instance().generate(5)};

    Logger::instance().log("connecting to server...");

    FixClient client;
    client.connectToServer(ConnectionPorts::OMEnginePort);

    Logger::instance().log("connected to server...");

    /* Construct dummy Fix */
    FixMessage fix;
    fix.setTag(FixTag::MsgType, "8");
    fix.setTag(FixTag::Side, "1");
    fix.setTag(FixTag::Currency, "GBP");
    fix.setTag(FixTag::OrderQty, "1");
    fix.setTag(FixTag::Price, "100.00");
    fix.setTag(FixTag::ExecTransType, "0"); /* NEW */
    fix.setTag(FixTag::ExecType, "0");
    fix.setTag(FixTag::SenderSubID, kTraderID);
    fix.setTag(FixTag::Trace, "FOBooking");
    /* TODO: - set product, productID, sending time, sender details, trader ID */

    while (true)
    {
        fix.setTag(FixTag::ClOrdID, UUID::instance().generate(15));

        client.broadcast(fix);
        sleep(1);
    }

    return 0;
}