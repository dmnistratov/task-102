include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS messages.proto)

add_library(proto ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto ${Protobuf_LIBRARIES})