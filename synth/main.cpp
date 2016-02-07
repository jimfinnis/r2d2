/**
 * @file main.cpp
 * @brief  Brief description of file.
 *
 */

#include "lib.h"

/*
 * Main program
 */


int main(int argc,char *argv[]){
    sonicAESSynth::init(argc>1?argv[1]:NULL);
    //    sonicAESSynth::testStdout();
    sonicAESSynth::initAudio();
    sonicAESSynth::testNotes();
    
    return 0;
}
