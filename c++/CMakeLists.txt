cmake_minimum_required(VERSION 3.10)
project(toy_lang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set GTest paths for macOS
set(GTEST_ROOT "/usr/local/opt/googletest")
set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
set(GTEST_LIBRARY_DIR "${GTEST_ROOT}/lib")

# Find GTest package
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(tokenizer_test
    tokenizer/tokenizer_test.cpp
    tokenizer/tokenizer.cpp
)
target_include_directories(tokenizer_test PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/tokenizer
)
target_link_libraries(tokenizer_test GTest::GTest GTest::Main pthread)
target_link_directories(tokenizer_test PRIVATE ${GTEST_LIBRARY_DIR})

add_executable(parser_test
    parser/parser_test.cpp
    parser/parser.cpp
    tokenizer/tokenizer.cpp
)
target_include_directories(parser_test PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/tokenizer
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
)
target_link_libraries(parser_test GTest::GTest GTest::Main pthread)
target_link_directories(parser_test PRIVATE ${GTEST_LIBRARY_DIR}) 