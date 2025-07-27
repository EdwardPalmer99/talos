/**
 * @file FixTag.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once


namespace Fix
{

enum class Tag : int
{
    ClOrdID = 11,
    Currency = 15,
    ExecID = 17,
    ExecTransType = 20,
    IDSource = 22,
    MsgType = 35,
    OrderQty = 38,
    Price = 44,
    SecurityID = 48,
    SendingTime = 52,
    Side = 54,
    TransactTime = 60,
};


} // namespace Fix