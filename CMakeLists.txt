cmake_minimum_required(VERSION 3.16)

set(target evserver)

project(${target} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE sources
	src/*.h
	src/*.cpp
)

add_executable(${target} ${sources})

target_include_directories(${target} PRIVATE src/)

target_link_libraries(${target} ev)

set_target_properties(${target} PROPERTIES
	                            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/)
