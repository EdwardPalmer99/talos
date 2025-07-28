/**
 * @file Exchange.cpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Exchange.hpp"
#include <iostream>
#include <unistd.h>


void Exchange::handleFixMessage(FixMessage message)
{
    std::string msgType = message.getValue(FixTag::MsgType);

    if (msgType != "8") /* TODO: - should handle; send alert instead */
    {
        throw std::runtime_error("received invalid msgType [" + msgType + "]");
    }

    std::cout << "Exchange ReceivedFix: [" << message.toString() << "]" << std::endl;

    /* Exchange sends-back a modified message to the OMSystem */
    FixMessage ackMessage(message);
    ackMessage.setTag(FixTag::MsgType, "AR");
    ackMessage.setTag(FixTag::ExecType, "0");

    FixClient sendToOMS(8080);
    sendToOMS.doSend(ackMessage);

    sleep(0.5);

    /* Exchange send back a Fill */
    FixMessage fillMessage(message);
    fillMessage.setTag(FixTag::MsgType, "AE");
    fillMessage.setTag(FixTag::ExecType, "2"); /* Fill */

    sendToOMS.doSend(ackMessage);

    /* TODO: - operate on separate thread */
    /* TODO: - have exchange's own DB here */
    /* TODO: - send an acknowledgement etc here */
}
