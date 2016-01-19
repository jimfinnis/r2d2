cmake_minimum_required(VERSION 2.6)
project(synth)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Jack REQUIRED)
find_package(Threads REQUIRED)

include_directories(${JACK_INCLUDE_DIR})
add_definitions(${JACK_DEFINITIONS})

set(SOURCES main.cpp)

add_executable(synth ${SOURCES})
target_link_libraries(synth ${JACK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_compile_options(synth PUBLIC "-pthread")
