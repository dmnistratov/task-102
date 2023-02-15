add_library(protoParser
    protobuf_parser/DelimitedMessagesStreamParser.hpp
    protobuf_parser/helpers.hpp
)

target_link_libraries(protoParser PUBLIC proto)