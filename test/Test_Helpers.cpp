#include <gtest/gtest.h>
#include <list>
#include <string>

#include "src/protobuf_parser/helpers.hpp"
#include "proto/messages.pb.h"

using TestTask::Messages::WrapperMessage;

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