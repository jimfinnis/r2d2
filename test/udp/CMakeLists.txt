cmake_minimum_required(VERSION 2.6)
project(udp)

set(ANGORT_DIR /home/white/angort)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${CMAKE_SOURCE_DIR})

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

add_words_files(udp.cpp)

set(SOURCE udpserver.cpp udpclient.cpp
${WORDFILELIST})

add_library(udp SHARED ${SOURCE})

add_custom_command(TARGET udp POST_BUILD
    COMMAND echo blah
    COMMAND cp $<TARGET_FILE:udp> udp.angso
)   


