#ifndef DELIMITED_MESSAGES_STREAM_PARSER_H
#define DELIMITED_MESSAGES_STREAM_PARSER_H

#include <list>
#include <memory>
#include <vector>

#include "parseDelimited.h"

template<typename MessageType>
class DelimitedMessagesStreamParser
{
    public:
        typedef std::shared_ptr<const MessageType> PointerToConstValue;
        std::list<PointerToConstValue> parse(const std::string& data){
            for (auto byte : data)
                m_buffer.push_back(byte);

            size_t bytesConsumed = 0;
            std::list<PointerToConstValue> messages;
            
            while(auto message = parseDelimited<MessageType>(m_buffer.data(), m_buffer.size(), &bytesConsumed)){
                messages.push_back(message);
                m_buffer.erase(m_buffer.begin(), m_buffer.begin() + bytesConsumed);
                bytesConsumed = 0;
            }

            return messages;
        };

    private:
        std::vector<char> m_buffer;
};

#endif  // DELIMITED_MESSAGES_STREAM_PARSER_H