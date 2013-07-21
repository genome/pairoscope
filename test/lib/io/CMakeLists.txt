cmake_minimum_required(VERSION 2.8)

# No lib yet, the io lib is header only
#set(TEST_LIBS io)

include_directories(${GTEST_INCLUDE_DIRS})

add_unit_tests(TestIoLib
    TestDepthArray.cpp
)
