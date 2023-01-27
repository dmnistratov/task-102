#ifndef SERIALIZE_DELIMITED_H
#define SERIALIZE_DELIMITED_H

#if GOOGLE_PROTOBUF_VERSION >= 3012004
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSizeLong())
#else
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSize())
#endif

#include <google/protobuf/io/coded_stream.h>

typedef std::vector<char> Data;
typedef std::shared_ptr<const Data> PointerToConstData;

template <typename Message> PointerToConstData serializeDelimited(const Message& msg)
{
    const size_t messageSize = PROTOBUF_MESSAGE_BYTE_SIZE(msg);
    const size_t headerSize = google::protobuf::io::CodedOutputStream::VarintSize32(messageSize);

    const PointerToConstData & result = std::make_shared<Data>(headerSize + messageSize);
    google::protobuf::uint8 * buffer = reinterpret_cast<google::protobuf::uint8*>(const_cast<char *>(&*result->begin()));
    
    google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(messageSize, buffer);
    msg.SerializeWithCachedSizesToArray(buffer + headerSize);

    return result;
}

#endif  // SERIALIZE_DELIMITED_H