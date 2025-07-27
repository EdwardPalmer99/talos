/**
 * @file FixMessage.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "FixMessage.hpp"
#include "FixTag.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace Fix
{

Message::Message(Value messageType) : _messageType(messageType)
{
}


// Message::Message(const std::string &message)
// {
//     /* TODO: - add error checking here and validation steps */
//     int index;
//     int offset = 0;

//     while ((index = message.find(';', offset)) != std::string::npos)
//     {
//         std::string tagValue = message.substr(offset, index);

//         /* TODO: - add a splitter here to separate the two and add to map */


//         offset += (index + 1);
//     }
// }


std::string Message::tagPair(int tag, Value value) const
{
    return (std::to_string(tag) + "=" + std::move(value) + ";");
}


const std::string &Message::toString() const
{
    if (!_message.empty())
    {
        return _message;
    }

    /* Body */
    std::string messageBody;
    messageBody += tagPair(35, _messageType);
    for (auto &[tag, value] : _valueForTag)
    {
        messageBody += tagPair(static_cast<int>(tag), value);
    }

    /* Header */
    _message += tagPair(8, "FIX.4.4");
    _message += tagPair(9, std::to_string(messageBody.size()));
    _message += messageBody;

    /* CheckSum: sum of all ASII characters except 10=xxx; tag */
    unsigned int count = 0;

    for (char c : _message)
    {
        count += static_cast<unsigned char>(c);
    }

    std::stringstream checksumStream;
    checksumStream << std::setw(3) << std::setfill('0') << std::to_string(count % 256);

    _message += tagPair(10, checksumStream.str());

    return _message;
}

} // namespace Fix