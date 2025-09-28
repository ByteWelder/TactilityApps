cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SOURCES "Source/*.c*")
file(GLOB HEADERS "Include/*.h*")

add_library(NotSoStdlib STATIC)

target_sources(NotSoStdlib PRIVATE ${SOURCES})
include_directories(NotSoStdlib "Include/")
target_include_directories(NotSoStdlib PUBLIC "Include/")

