#ifndef SRC_PROTOBUF_PARSER_HELPERS_HPP
#define SRC_PROTOBUF_PARSER_HELPERS_HPP

#include <string>
#include <memory>
#include <google/protobuf/io/coded_stream.h>

#if GOOGLE_PROTOBUF_VERSION >= 3012004
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSizeLong())
#else
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSize())
#endif

#define PROTOBUF_VARINT_BYTE_SIZE(length) (google::protobuf::io::CodedOutputStream::VarintSize32((length)))

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

/*!
 * \brief Расшифровывает сообщение, предваренное длиной из массива байтов.
 *
 * \tparam Message Тип сообщения, для работы с которым предназначена данная функция.
 *
 * \param data Указатель на буфер данных.
 * \param size Размер буфера данных.
 * \param bytesConsumed Количество байт, которое потребовалось для расшифровки сообщения в случае успеха.
 *
 * \return Умный указатель на сообщение. Если удалось расшифровать сообщение, то он не пустой.
 */

template<typename Message>
std::shared_ptr<Message> parseDelimited(const void* data, size_t size, size_t* bytesConsumed = 0){
    if (!data || size == 0)
        return nullptr;

    uint32_t length;
    google::protobuf::io::CodedInputStream protoStream(static_cast<const uint8_t *>(data), size);
    protoStream.ReadVarint32(&length);

    if (length + PROTOBUF_VARINT_BYTE_SIZE(length) > size || length == 0)
        return nullptr;

    auto limit = protoStream.PushLimit(length);

    auto message = std::make_shared<Message>();
    
    if(message->ParseFromCodedStream(&protoStream) && protoStream.ConsumedEntireMessage()) {
        if (bytesConsumed)
            *bytesConsumed = length + PROTOBUF_VARINT_BYTE_SIZE(length);
            
        protoStream.PopLimit(limit);
        return message;
    }
    return nullptr;
};

#endif // SRC_PROTOBUF_PARSER_HELPERS_HPP