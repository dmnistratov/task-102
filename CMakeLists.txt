cmake_minimum_required(VERSION 3.10)

project(cpp-protobuf-parser)

add_subdirectory(proto)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)