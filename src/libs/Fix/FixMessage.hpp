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


class FixMessage
{
public:
    using Tag = int;
    using Value = std::string;
    using TagValue = std::pair<Tag, Value>;

    FixMessage() = default;

    /* Construct from a raw-FIX string */
    FixMessage(const std::string &message);

    /* Update value for a specific tag */
    void setTag(Tag tag, Value value);

    /* Remove a tag from a FIX message */
    void eraseTag(Tag tag);

    /* Returns true if tag is present in FIX message */
    [[nodiscard]] bool hasTag(Tag tag) const;

    /* Returns the value for a tag or an empty string if not present */
    [[nodiscard]] Value getValue(Tag tag) const;

    /* Converts to a std::string */
    [[nodiscard]] const std::string &toString() const;

protected:
    std::string constructTagValuePair(Tag tag, Value value) const;

    TagValue extractTagValuePair(const std::string &tagValue) const;

private:
    /* Stores constructed message */
    mutable std::string _message;
    std::unordered_map<Tag, Value> _valueForTag;
};
