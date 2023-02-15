add_executable(protoParser_test
    Test_Helpers.cpp
    Test_DelimitedMessagesStreamParser.cpp
)

target_include_directories(protoParser_test PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(protoParser_test PRIVATE "${CMAKE_BINARY_DIR}")

target_link_libraries(protoParser_test
    gtest
    gtest_main
    protoParser
)

include(GoogleTest)
gtest_discover_tests(protoParser_test)