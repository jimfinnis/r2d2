/**
 * @file synth.cpp
 * @brief  Brief description of file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <angort/angort.h>

#include "../../synth/lib.h"

using namespace angort;

%name synth
%shared

%wordargs init y (filename --) initialise with pre-start file
{
    sonicAESSynth::init(p0);
}

%word initaudio (--) initialise audio
{
    sonicAESSynth::initAudio();
}

%word shutdownaudio (--) close down audio
{
    sonicAESSynth::shutdownAudio();
}

%wordargs runfile s (filename --) run a file
{
    sonicAESSynth::runFile(p0);
}


%wordargs cmd s (string --) issue a command
{
    sonicAESSynth::cmd(p0);
}


%init
{
    fprintf(stderr,"Initialising SYNTH plugin, %s %s\n",__DATE__,__TIME__);
}
