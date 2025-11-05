cmake_minimum_required(VERSION 3.1)
project(timelog)

set(CMAKE_CXX_STANDARD 17)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

find_package(ZLIB REQUIRED)

add_library(timelog
    src/cli.cpp
    src/compressor.cpp
    src/manifest.cpp
    src/packer.cpp
    src/unpacker.cpp
)

target_include_directories(timelog PUBLIC include)

target_link_libraries(timelog
    PRIVATE
        OpenSSL::SSL 
        OpenSSL::Crypto
        pthread
        ZLIB::ZLIB
)

add_executable(timelog_cli src/main.cpp)

target_link_libraries(timelog_cli PRIVATE timelog)