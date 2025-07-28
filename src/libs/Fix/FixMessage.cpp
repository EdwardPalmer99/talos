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


FixMessage::FixMessage(const std::string &message)
{
    _message = message; /* Already constructed */

    std::size_t iCurr = 0;
    std::size_t iNext = 0;
    while ((iNext = message.find(";", iCurr)) != std::string::npos)
    {
        auto substring = message.substr(iCurr, iNext - iCurr);

        TagValue tagValue = extractTagValuePair(substring);
        setTag(tagValue.first, tagValue.second);

        iCurr = iNext + 1;
    }
}


bool FixMessage::hasTag(Tag tag) const
{
    return (_valueForTag.find(tag) != _valueForTag.end());
}


void FixMessage::setTag(Tag tag, Value value)
{
    switch (tag)
    {
        case 8: /* Ignore special header tags */
        case 9:
        case 10:
            break;
        default:
            _valueForTag[tag] = std::move(value);
            _message.clear(); /* Needs to be recomputed */
            break;
    }
}


void FixMessage::eraseTag(Tag tag)
{
    auto iter = _valueForTag.find(tag);
    if (iter == _valueForTag.end())
    {
        return;
    }

    _valueForTag.erase(iter);
    _message.clear(); /* Needs to be recomputed */
}


FixMessage::Value FixMessage::getValue(Tag tag) const
{
    auto iter = _valueForTag.find(tag);
    return (iter != _valueForTag.end() ? iter->second : "");
}


FixMessage::TagValue FixMessage::extractTagValuePair(const std::string &tagValue) const
{
    std::size_t iSeparator = tagValue.find("=");

    if (iSeparator == std::string::npos)
    {
        throw std::runtime_error("missing expected separator '='");
    }

    std::string tag = tagValue.substr(0, iSeparator);
    std::string value = tagValue.substr(iSeparator + 1);

    return TagValue(std::stoi(tag), value);
}


std::string FixMessage::constructTagValuePair(Tag tag, Value value) const
{
    return (std::to_string(tag) + "=" + std::move(value) + ";");
}


const std::string &FixMessage::toString() const
{
    if (!_message.empty())
    {
        return _message;
    }

    /* Body */
    std::string messageBody;
    for (auto &[tag, value] : _valueForTag)
    {
        messageBody += constructTagValuePair(tag, value);
    }

    /* Header */
    _message += constructTagValuePair(8, "FIX.4.4");
    _message += constructTagValuePair(9, std::to_string(messageBody.size()));
    _message += messageBody;

    /* CheckSum: sum of all ASII characters except 10=xxx; tag */
    unsigned int count = 0;

    for (char c : _message)
    {
        count += static_cast<unsigned char>(c);
    }

    std::stringstream checksumStream;
    checksumStream << std::setw(3) << std::setfill('0') << std::to_string(count % 256);

    _message += constructTagValuePair(10, checksumStream.str());

    return _message;
}
