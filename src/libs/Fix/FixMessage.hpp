/**
 * @file FixTag.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "FixTag.hpp"
#include <string>
#include <unordered_map>

namespace Fix
{

class Message
{
public:
    using Value = std::string;

    Message() = delete;
    Message(Value messageType = "8");

    /* Construct from a raw-FIX string */
    // Message(std::string message);

    [[nodiscard]] inline bool hasPair(Tag tag) const;

    void inline setPair(Tag tag, Value value);

    [[nodiscard]] Value inline getValue(Tag tag) const;

    [[nodiscard]] const std::string &toString() const;

protected:
    std::string tagPair(int tag, Value value) const;

private:
    /* Stores constructed message */
    mutable std::string _message;

    Value _messageType; /* tag 35 */
    std::unordered_map<Tag, Value> _valueForTag;
};


bool Message::hasPair(Tag tag) const
{
    return (_valueForTag.find(tag) != _valueForTag.end());
}


void Message::setPair(Tag tag, Value value)
{
    _valueForTag[tag] = std::move(value);
    _message.clear(); /* Needs to be recomputed */
}


Message::Value Message::getValue(Tag tag) const
{
    auto iter = _valueForTag.find(tag);
    return (iter != _valueForTag.end() ? iter->second : "");
}

} // namespace Fix
