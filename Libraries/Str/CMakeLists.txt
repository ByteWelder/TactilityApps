cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SOURCES "Source/*.c*")
file(GLOB HEADERS "Include/*.h*")

add_library(Str STATIC)

target_sources(Str PRIVATE ${SOURCES})
include_directories(Str "Include/")
target_include_directories(Str PUBLIC "Include/")

