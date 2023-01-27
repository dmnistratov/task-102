#include <gtest/gtest.h>
#include <list>
#include <string>

#include "src/DelimitedMessagesStreamParser.h"
#include "src/parseDelimited.h"
#include "src/serializeDelimited.h"
#include "proto/messages.pb.h"

using TestTask::Messages::WrapperMessage;
typedef DelimitedMessagesStreamParser<WrapperMessage> Parser;

// Tests parseDelimited function on some trivial cases.
TEST(ParseDelimited, Trivial){
    WrapperMessage message;
    PointerToConstData data;
    std::shared_ptr<WrapperMessage> parsedMessage;

    // Tests RequestForFastResponse.
    message.mutable_request_for_fast_response();
    data = serializeDelimited<WrapperMessage>(message);
    parsedMessage = parseDelimited<WrapperMessage>(data->data(), data->size());
    EXPECT_EQ(message.SerializeAsString(), parsedMessage->SerializeAsString());
    message.Clear();

    // Tests RequestForSlowResponse.
    message.mutable_request_for_slow_response()->set_time_in_seconds_to_sleep(1);
    data = serializeDelimited<WrapperMessage>(message);
    parsedMessage = parseDelimited<WrapperMessage>(data->data(), data->size());
    EXPECT_EQ(message.SerializeAsString(), parsedMessage->SerializeAsString());
    message.Clear();

    // Tests FastResponse.
    message.mutable_fast_response()->set_current_date_time("19851019T050107.333");
    data = serializeDelimited<WrapperMessage>(message);
    parsedMessage = parseDelimited<WrapperMessage>(data->data(), data->size());
    EXPECT_EQ(message.SerializeAsString(), parsedMessage->SerializeAsString());
    message.Clear();


    // Tests SlowResponse.
    message.mutable_slow_response()->set_connected_client_count(1);
    data = serializeDelimited<WrapperMessage>(message);
    parsedMessage = parseDelimited<WrapperMessage>(data->data(), data->size());
    EXPECT_EQ(message.SerializeAsString(), parsedMessage->SerializeAsString());
    message.Clear();
}

// Tests parseDelimited function on empty data.
TEST(ParseDelimited, EmptyData){
    std::shared_ptr<WrapperMessage> parsedMessage;

    parsedMessage = parseDelimited<WrapperMessage>(nullptr, 0);
    EXPECT_EQ(nullptr, parsedMessage);
    
    std::string data = "";
    parsedMessage = parseDelimited<WrapperMessage>(data.data(), data.size());
    EXPECT_EQ(nullptr, parsedMessage);
}

// Tests parseDelimited funcion length-prefixed.
TEST(ParseDelimited, LengthPrefixed){
    WrapperMessage message;
    std::shared_ptr<WrapperMessage> parsedMessage;
    std::string data = "";
    PointerToConstData messageData;

    // Tests string as message without length-prefixed.
    data = "String test";
    parsedMessage = parseDelimited<WrapperMessage>(data.data(), data.size());
    EXPECT_EQ(nullptr, parsedMessage);
    
    // Tests message without length-prefixed.
    message.mutable_request_for_fast_response();
    message.SerializeToString(&data);
    parsedMessage = parseDelimited<WrapperMessage>(data.data(), data.size());
    EXPECT_EQ(nullptr, parsedMessage);

    // Tests message with length-prefixed.
    messageData = serializeDelimited(message);
    parsedMessage = parseDelimited<WrapperMessage>(messageData->data(), messageData->size());
    EXPECT_EQ(message.SerializeAsString(), parsedMessage->SerializeAsString());
}

// Tests parseDelimited funcion size.
TEST(PraseDelimited, Size){
    WrapperMessage message;
    std::shared_ptr<WrapperMessage> parsedMessage;
    std::string data = "";
    PointerToConstData messageData;

    // Tests with size == 0.
    parsedMessage = parseDelimited<WrapperMessage>(data.data(), data.size());
    EXPECT_EQ(nullptr, parsedMessage);

    // Test with length > size.
    message.mutable_request_for_fast_response();
    messageData = serializeDelimited(message);
    parsedMessage = parseDelimited<WrapperMessage>(messageData->data(), messageData->size() / 2);
    EXPECT_EQ(nullptr, parsedMessage);

    // Tests with length != (size - length).
    parsedMessage = parseDelimited<WrapperMessage>(messageData->data(), messageData->size() * 2);
    EXPECT_EQ(message.SerializeAsString(), parsedMessage->SerializeAsString());
}

// Tests parseDelimited bytesConsumed.
TEST(PraseDelimited, bytesConsumed){
    WrapperMessage message;
    std::shared_ptr<WrapperMessage> parsedMessage;
    std::string data = "";
    PointerToConstData messageData;
    size_t bytesConsumed = 0;
    std::string buffer;

    // Test bytesConsumed with empty message.
    parsedMessage = parseDelimited<WrapperMessage>(data.data(), data.size(), &bytesConsumed);
    EXPECT_EQ(0, bytesConsumed);

    // Test bytesConsumed on trivial case. 
    message.mutable_request_for_fast_response();
    messageData = serializeDelimited(message);
    parsedMessage = parseDelimited<WrapperMessage>(messageData->data(), messageData->size(), &bytesConsumed);
    EXPECT_EQ(3, bytesConsumed);
    bytesConsumed = 0;

    // Tests bytesConsumed on multiple messages.
    buffer.append(messageData->begin(), messageData->end());
    buffer.append(messageData->begin(), messageData->end());
    parsedMessage = parseDelimited<WrapperMessage>(buffer.data(), buffer.size(), &bytesConsumed);
    EXPECT_EQ(3, bytesConsumed);
}

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