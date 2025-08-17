/**
 * @file ClientApp.cpp
 * @author Edward Palmer
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "OrderGenerator.hpp"
#include "logger/Logger.hpp"
#include "utilities/UUID.hpp"
#include <chrono>
#include <stdexcept>
#include <string>
#include <unistd.h>


void OrderGenerator::sendNewOrders(std::size_t nOrders, std::size_t delayMS)
{
    auto sockets = _portSocketMappings.getSockets();
    if (sockets.empty())
    {
        Logger::instance().error("No active sockets.");
        return;
    }

    for (std::size_t iOrder = 0; iOrder < nOrders; ++iOrder)
    {
        auto newOrder = buildNewOrder();

        if (delayMS)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMS));
        }

        for (auto socket : sockets)
        {
            sendFixMessage(newOrder, socket);
        }
    }
}


FixMessage OrderGenerator::buildNewOrder()
{
    FixMessage dummyOrder;

    dummyOrder.setTag(FixTag::MsgType, "D");
    dummyOrder.setTag(FixTag::Side, "1");
    dummyOrder.setTag(FixTag::Currency, "GBP");
    dummyOrder.setTag(FixTag::OrderQty, "1");
    dummyOrder.setTag(FixTag::Price, "100.00");
    dummyOrder.setTag(FixTag::ExecTransType, "0");
    dummyOrder.setTag(FixTag::ExecTransType, "0"); // 20=0
    dummyOrder.setTag(FixTag::ExecType, "0");      // 150=0
    dummyOrder.setTag(FixTag::OrderQty, "0");      // 39=0 (New)
    dummyOrder.setTag(FixTag::SenderSubID, "OrderGenerator");
    dummyOrder.setTag(FixTag::ClOrdID, UUID::instance().generate(15)); /* New unique ID for client order */
    /* TODO: - add security and other tags */
    /* TODO: - add tag 60 as current time */

    return dummyOrder;
}
