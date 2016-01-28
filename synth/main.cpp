/**
 * @file main.cpp
 * @brief  Brief description of file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <jack/jack.h>

#include "parser.h"

jack_port_t *output_port;

// table of midi note "frequencies" adjusted by sample rate
jack_default_audio_sample_t note_frqs[128]; 

// sample rate 
double samprate = 0;

// various other globals
jack_default_audio_sample_t amp=0.0;


// mutex used for message passing. This MUST NOT lock the process
// thread! We do, however, keep the lock (using trylock) until
// the command completes, which typically takes many runs of process()
// because a command plays an entire sound - this is a blocking synth!

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// this is the command data, which sends commands from the main program
// to the process() thread
Synth *cmdPending=NULL;

/*
 * JACK callbacks
 */

int process(jack_nframes_t nframes, void *arg){
    // set when we are running a command and reset when it ends.
    // ONLY USED IN THIS THREAD.
    static Synth *cmdRunning=NULL;
    
    jack_default_audio_sample_t *out = 
          (jack_default_audio_sample_t *)jack_port_get_buffer(output_port,
                                                              nframes);
    
    // check pending commands
    if(pthread_mutex_trylock(&lock)==0){
        // process pending command here. Currently the only command is
        // "bip"
        if(cmdPending){
            cmdRunning=cmdPending;
            cmdPending=NULL; 
        } else {
            // note that if a command was received,
            // we keep the lock until the command completes
            pthread_mutex_unlock(&lock);
        }
        
    }
    
    // make noises
    if(cmdRunning){
        cmdRunning->update(nframes);
        double *outbuf = cmdRunning->getout();
        for(int i=0;i<nframes;i++){
            out[i] = outbuf[i];
        }
        // detect command completion, and unlock the thread if
        // complete.
        if( cmdRunning->done){
            Synth *n = cmdRunning->next;
            delete cmdRunning;
            cmdRunning=n; // run next (if there is one)
            if(!cmdRunning) // if not, unlock.
                pthread_mutex_unlock(&lock);
        }
    } else {
        for(int i=0;i<nframes;i++){
            out[i]=0;
        }
    }
        
    return 0;
}


void jack_shutdown(void *arg)
{
    exit(1);
}

int srate(jack_nframes_t nframes, void *arg)
{
    printf("the sample rate is now %" PRIu32 "/sec\n", nframes);
    samprate = nframes;
    return 0;
}



/*
 * Main program
 */

Parser parser;

// send commands to the process. Will block until the process thread
// has completed any command.
void cmd(const char *buf){
    Synth *s = parser.parse(buf);
    printf("Waiting for lock..\n");
    pthread_mutex_lock(&lock);
    cmdPending=s;
    pthread_mutex_unlock(&lock);
    printf("Command sent..\n");
}


int main(int argc,char *argv[]){
    jack_client_t *client;
    
    // create a mutex used to pass data from main thread to process
    // thread and vice versa.

    if (!(client = jack_client_open("r2d2", JackNullOption, NULL))){
        fprintf(stderr, "jack server not running?\n");
        return 1;
    }
    
    // set callbacks
    jack_set_process_callback(client, process, 0);
    jack_set_sample_rate_callback(client, srate, 0);
    jack_on_shutdown(client, jack_shutdown, 0);
    
    samprate = (double)jack_get_sample_rate(client);
    
    output_port = jack_port_register(
                                     client, 
                                     "r2d2", 
                                     JACK_DEFAULT_AUDIO_TYPE, 
                                     JackPortIsOutput, 0);
    if (jack_activate(client)) {
        fprintf(stderr, "cannot activate client");
        return 1;
    }
    
    jack_connect(client,"r2d2:r2d2","system:playback_1");
    jack_connect(client,"r2d2:r2d2","system:playback_2");
    
    printf("Active.\n");
    while(1){
        cmd("beep 440;beep 220;beep 1302;beep 1212;beep 554;beep 832;beep 100");
        sleep(1);
    }
    
    jack_client_close(client);
    exit(0);
}
