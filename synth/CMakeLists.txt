cmake_minimum_required(VERSION 2.6)
project(synth)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Jack REQUIRED)
find_package(Threads REQUIRED)

include_directories(${JACK_INCLUDE_DIR} ${CMAKE_BINARY_DIR} .)
add_definitions(${JACK_DEFINITIONS} -Wall)

set(SOURCES lib.cpp WaveTableOsc.cpp tokeniser.cpp tokens.cpp synth.cpp
    parser.cpp synthdef.cpp LPF.cpp)

add_custom_command(
    OUTPUT tokens.cpp tokens.h
    DEPENDS tokens
    COMMAND python ${CMAKE_SOURCE_DIR}/gentoks ${CMAKE_SOURCE_DIR}/tokens tokens)


add_library(synth ${SOURCES})
target_compile_options(synth PUBLIC "-pthread")

add_executable(test main.cpp)
add_dependencies(test synth)
target_link_libraries(test synth ${JACK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_compile_options(test PUBLIC "-pthread")
