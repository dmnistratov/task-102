#ifndef PARSE_DELIMITED_H
#define PARSE_DELIMITED_H

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

#define PROTOBUF_VARINT_BYTE_SIZE(length) (google::protobuf::io::CodedOutputStream::VarintSize32((length)))

#include <string>
#include <memory>
#include <google/protobuf/io/coded_stream.h>

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

#endif // PARSE_DELIMITED_H