/**
 * @file TestFixMessage.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <fix/FixMessage.hpp>
#include <fix/FixTag.hpp>
#include <gtest/gtest.h>
#include <iostream>

namespace Fix
{

class FixMessageTest : public testing::Test
{
protected:
    void SetUp() override
    {
        FixMessage message;
        message.setTag(35, "8");
        message.setTag(FixTag::Side, "1");
        message.setTag(FixTag::Price, "100.00");

        _message = std::move(message);
    }

    FixMessage _message;
};


TEST_F(FixMessageTest, CheckHasTag)
{
    EXPECT_TRUE(_message.hasTag(FixTag::Side));
    EXPECT_TRUE(_message.hasTag(FixTag::Price));

    EXPECT_FALSE(_message.hasTag(FixTag::TransactTime));
}


TEST_F(FixMessageTest, CheckGetPair)
{
    EXPECT_EQ(_message.getValue(FixTag::Side), "1");
    EXPECT_EQ(_message.getValue(FixTag::Price), "100.00");

    EXPECT_EQ(_message.getValue(FixTag::TransactTime), "");
}


TEST_F(FixMessageTest, CheckSetPair)
{
    _message.setTag(FixTag::Side, "2");
    EXPECT_EQ(_message.getValue(FixTag::Side), "2");
}


TEST_F(FixMessageTest, CheckToString)
{
    std::string fix = _message.toString();

    std::string expectedFix{"8=FIX.4.4;9=20;44=100.00;54=1;35=8;10=151;"};
    EXPECT_EQ(fix, expectedFix);
}


TEST_F(FixMessageTest, CheckFromString)
{
    FixMessage newMessage(_message.toString());

    EXPECT_EQ(newMessage.getValue(35), "8");
    EXPECT_EQ(newMessage.getValue(FixTag::Side), "1");
    EXPECT_EQ(newMessage.getValue(FixTag::Price), "100.00");
}


} // namespace Fix