/**
 * @file FixTag.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once


enum FixTag
{
    ClOrdID = 11,
    Currency = 15,
    ExecID = 17,
    ExecTransType = 20,
    ExecType = 150,
    IDSource = 22,
    MsgSeqNo = 34,
    MsgType = 35,
    OrderQty = 38,
    Price = 44,
    SecurityID = 48,
    SendingTime = 52, /* Message transmission time UTC */
    Side = 54,
    TransactTime = 60,
    SenderCompID = 49, /* Firm sending message */
    SenderSubID = 50,  /* Specific message originator (trader, desk, ...)*/

    /* User tags */
    Trace = 10000,

};
