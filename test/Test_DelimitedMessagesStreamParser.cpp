#include <gtest/gtest.h>
#include <list>
#include <string>

#include "src/protobuf_parser/DelimitedMessagesStreamParser.hpp"
#include "proto/messages.pb.h"

using TestTask::Messages::WrapperMessage;
typedef DelimitedMessagesStreamParser<WrapperMessage> Parser;

// Tests DelimitedMessagesStreamParser on trivial cases.
TEST(StreamParser, Trivial){
    WrapperMessage message;
    WrapperMessage message2;
    PointerToConstData messageData;
    std::string buffer;
    Parser parser;
    std::list<DelimitedMessagesStreamParser<WrapperMessage>::PointerToConstValue> messages;

    // Tests RequestForFastResponse.
    message.mutable_request_for_fast_response();
    messageData = serializeDelimited<WrapperMessage>(message);
    messages = parser.parse(std::string(messageData->begin(), messageData->end()));
    EXPECT_EQ(1, messages.size());
    EXPECT_EQ(message.SerializeAsString(), messages.front()->SerializeAsString());
    message.Clear();

    // Tests RequestForSlowResponse.
    message.mutable_request_for_slow_response()->set_time_in_seconds_to_sleep(1);
    messageData = serializeDelimited<WrapperMessage>(message);
    messages = parser.parse(std::string(messageData->begin(), messageData->end()));
    EXPECT_EQ(1, messages.size());
    EXPECT_EQ(message.SerializeAsString(), messages.front()->SerializeAsString());
    message.Clear();

    // Tests FastResponse.
    message.mutable_fast_response()->set_current_date_time("19851019T050107.333");
    messageData = serializeDelimited<WrapperMessage>(message);
    messages = parser.parse(std::string(messageData->begin(), messageData->end()));
    EXPECT_EQ(1, messages.size());
    EXPECT_EQ(message.SerializeAsString(), messages.front()->SerializeAsString());
    message.Clear();


    // Tests SlowResponse.
    message.mutable_slow_response()->set_connected_client_count(1);
    messageData = serializeDelimited<WrapperMessage>(message);
    messages = parser.parse(std::string(messageData->begin(), messageData->end()));
    EXPECT_EQ(1, messages.size());
    EXPECT_EQ(message.SerializeAsString(), messages.front()->SerializeAsString());
    message.Clear();

    // Tests multiple identical messages.
    message.mutable_request_for_fast_response();
    messageData = serializeDelimited<WrapperMessage>(message);
    buffer.append(messageData->begin(), messageData->end());
    buffer.append(messageData->begin(), messageData->end());
    messages = parser.parse(buffer);
    EXPECT_EQ(2, messages.size());
    EXPECT_EQ(message.SerializeAsString(), messages.front()->SerializeAsString());
    EXPECT_EQ(message.SerializeAsString(), messages.back()->SerializeAsString());
    message.Clear();
    buffer.clear();

    // Tests miltiple different messages.
    message.mutable_request_for_fast_response();
    messageData = serializeDelimited<WrapperMessage>(message);
    buffer.append(messageData->begin(), messageData->end());
    message2.mutable_slow_response()->set_connected_client_count(1);
    messageData = serializeDelimited<WrapperMessage>(message2);
    buffer.append(messageData->begin(), messageData->end());
    messages = parser.parse(buffer);
    EXPECT_EQ(2, messages.size());
    EXPECT_EQ(message.SerializeAsString(), messages.front()->SerializeAsString());
    EXPECT_EQ(message2.SerializeAsString(), messages.back()->SerializeAsString());
    message.Clear();
}

// Tests DelimitedMessagesStreamParser on empty data.
TEST(StreamParser, EmptyData){
    Parser parser;
    std::list<DelimitedMessagesStreamParser<WrapperMessage>::PointerToConstValue> messages;

    std::string data = "";
    messages = parser.parse(data);
    EXPECT_TRUE(messages.empty());
}

// Tests DelimitedMessagesStreamParser with wrong buffer data.
TEST(StreamParser, WrongData){
    std::string data;
    Parser parser;
    std::list<DelimitedMessagesStreamParser<WrapperMessage>::PointerToConstValue> messages;


    // Test parser on message without length-prefixed
    data = "Hello, world!";
    messages = parser.parse(data);
    EXPECT_TRUE(messages.empty());

    data = "\x02\x05\x02";
    messages = parser.parse(data);
    EXPECT_TRUE(messages.empty());
}