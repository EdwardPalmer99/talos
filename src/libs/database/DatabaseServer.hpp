/**
 * @file DatabaseServer.hpp
 * @author Edward Palmer
 * @date 2025-08-04
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "socket/FixServer.hpp"
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>


class DatabaseServer : public FixServer
{
public:
    DatabaseServer(Port dbPort) : FixServer(dbPort) {}

protected:
    struct OrderRecord
    {
        std::string clOrdID;     // 11
        std::string orderStatus; // 39: current execution report: 0=New, 1=PartialFill, 2=Fill, 4=Cancelled; 8=Rejected
        std::string side;        // 54: 1=Buy, 2=Sell
        std::string currency;    // 15: GBP, USD, EUR
        std::string orderQty;    // 38
        std::string price;       // 44
        std::string execType;    // 150: specific execution report
        std::string creationTime;
        std::string lastUpdateTime;
    };

    /* Returns pointer to OrderRecord or nullptr if not found */
    OrderRecord *lookupOrderRecord(std::string originalClOrdID);

    /* 35=D message */
    void handleNewOrder(FixMessage message, SocketFD socket);

    /* 35=8 message */
    void handleExecutionReport(FixMessage message, SocketFD socket);

    /* Hooks */
    void onRegisterMsgTypes() override;

private:
    mutable std::shared_mutex _orderRecordMutex;
    std::unordered_map<std::string, std::unique_ptr<OrderRecord>> _orderRecords;
};