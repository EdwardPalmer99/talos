/**
 * @file DatabaseServer.cpp
 * @author Edward Palmer
 * @date 2025-08-04
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "DatabaseServer.hpp"
#include "logger/Logger.hpp"


void DatabaseServer::handleFixMessage(FixMessage fixMsg, SocketFD clientSocket)
{
    std::string msgType(fixMsg.getValue(FixTag::MsgType));

    if (msgType == "D") /* New */
    {
        createOrderRecord(std::move(fixMsg));
    }
    else if (msgType == "8") /* Execution report */
    {
        updateOrderRecord(std::move(fixMsg));
    }
    else if (msgType == "QR") /* Query Record (custom type) for NetAdmin: send back FIX with details */
    {
        handleDatabaseQuery(std::move(fixMsg), clientSocket);
    }
    else
    {
        Logger::instance().error("Received FixMsg with invalid MsgType " + msgType);
    }
}


void DatabaseServer::handleDatabaseQuery(FixMessage queryFixMsg, SocketFD netAdminSocket)
{
    std::string clOrdID(queryFixMsg.getValue(FixTag::ClOrdID));

    FixMessage responseFix;
    responseFix.setTag(FixTag::MsgType, "QR");
    responseFix.setTag(FixTag::ClOrdID, clOrdID);

    OrderRecord *orderRecord = lookupOrderRecord(clOrdID);
    if (!orderRecord)
    {
        Logger::instance().error("ClOrdID " + clOrdID + " not found in map");
        /* No details */
    }
    else
    {
        responseFix.setTag(FixTag::OrdStatus, orderRecord->orderStatus);
        responseFix.setTag(FixTag::Side, orderRecord->side);
        responseFix.setTag(FixTag::Currency, orderRecord->currency);
        responseFix.setTag(FixTag::OrderQty, orderRecord->orderQty);
        responseFix.setTag(FixTag::Price, orderRecord->price);
        responseFix.setTag(FixTag::ExecType, orderRecord->execType);
        /* TODO: - may need to create custom tags for lastUpdateTime, creationTime */
    }

    sendFixMessage(std::move(responseFix), netAdminSocket);
}


void DatabaseServer::createOrderRecord(FixMessage fixMsg)
{
    std::string clOrdID(fixMsg.getValue(FixTag::ClOrdID));

    if (lookupOrderRecord(clOrdID))
    {
        Logger::instance().error("Detected duplicate ClOrdID " + clOrdID);
        return;
    }

    auto orderRecord = std::make_unique<OrderRecord>();

    orderRecord->clOrdID = clOrdID;
    orderRecord->orderStatus = "0"; /* New */
    orderRecord->side = fixMsg.getValue(FixTag::Side);
    orderRecord->currency = fixMsg.getValue(FixTag::Currency);
    orderRecord->orderQty = fixMsg.getValue(FixTag::OrderQty);
    orderRecord->price = fixMsg.getValue(FixTag::Price);
    orderRecord->execType = "0";
    orderRecord->lastUpdateTime = orderRecord->creationTime = nowUTC();

    Logger::instance().info("Created new order record for ClOrdID " + clOrdID);

    std::unique_lock lock(_orderRecordMutex);
    _orderRecords[clOrdID] = std::move(orderRecord);
}


void DatabaseServer::updateOrderRecord(FixMessage fixMsg)
{
    std::string clOrdID(fixMsg.getValue(FixTag::ClOrdID));

    auto *orderRecord = lookupOrderRecord(clOrdID);
    if (!orderRecord)
    {
        Logger::instance().error("No order record found for ClOrdID " + clOrdID);
        return;
    }

    std::string newOrdStatus(fixMsg.getValue(FixTag::OrdStatus));

    Logger::instance().info("Updating " + clOrdID + ": " + orderRecord->orderStatus + " => " + newOrdStatus);

    std::unique_lock lock(_orderRecordMutex);
    orderRecord->orderStatus = newOrdStatus;
    orderRecord->lastUpdateTime = nowUTC();
}


DatabaseServer::OrderRecord *DatabaseServer::lookupOrderRecord(std::string originalClOrdID)
{
    std::shared_lock lock(_orderRecordMutex);
    auto iter = _orderRecords.find(originalClOrdID);

    return iter != _orderRecords.end() ? iter->second.get() : nullptr;
}
