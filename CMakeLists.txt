cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0016 NEW)

project(cpp-protobuf-parser VERSION 1.0.0 DESCRIPTION "Разбор потока length-prefixed Protobuf сообщений на C++")

add_subdirectory(proto)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)