cmake_minimum_required(VERSION 2.6)
project(synth)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(ANGORT_DIR /home/white/angort)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(Jack REQUIRED)
find_library(SYNTHLIB synth PATHS ../../synth/build)

include_directories(${CMAKE_SOURCE_DIR})
add_definitions(${JACK_DEFINITIONS} -Wall)

set(WORDFILELIST)
# this macro generates a custom build step which takes the files passed in,
# passes them through Angort's 'makeWords.pl' script, and compiles *those*.
# The files generated are added to WORDFILELIST, which should then be put
# into the SOURCES. So, Angort word files should be added with this.
macro(add_words_files)
    add_custom_command(
        OUTPUT wordslist
        COMMAND rm wordslist
        )
    foreach(file ${ARGV})
        add_custom_command(
            OUTPUT ${file}.cpp
            DEPENDS ${file}
            COMMAND pwd
            COMMAND perl  ${ANGORT_DIR}/makeWords.pl  ${CMAKE_SOURCE_DIR}/${file}>${file}.cpp
            COMMAND cat words >>wordslist
        )
        set(WORDFILELIST ${WORDFILELIST} ${file}.cpp)
    endforeach(file)
endmacro(add_words_files)

add_words_files(synth.cpp)

set(SOURCE ${WORDFILELIST})

add_library(synth SHARED ${SOURCE})

target_link_libraries(synth ${SYNTHLIB} ${JACK_LIBRARIES})

add_custom_command(TARGET synth POST_BUILD
    COMMAND cp $<TARGET_FILE:synth> synth.angso
)   


