cmake_minimum_required(VERSION 2.6)
project(synth)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Jack REQUIRED)
find_package(Threads REQUIRED)

include_directories(${JACK_INCLUDE_DIR} ${CMAKE_BINARY_DIR} .)
add_definitions(${JACK_DEFINITIONS} -Wall)

set(SOURCES main.cpp WaveTableOsc.cpp tokeniser.cpp tokens.cpp
    parser.cpp synthdef.cpp LPF.cpp)

add_custom_command(
    OUTPUT tokens.cpp tokens.h
    DEPENDS tokens
    COMMAND python ${CMAKE_SOURCE_DIR}/gentoks ${CMAKE_SOURCE_DIR}/tokens tokens)

add_executable(synth ${SOURCES})
target_link_libraries(synth ${JACK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_compile_options(synth PUBLIC "-pthread")
