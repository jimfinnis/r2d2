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

#include "WaveTableOsc.h"
#include "SinOsc.h"

SinOsc sosc1,sosc2;
double *SinOsc::table=NULL;

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
int cmdPending=0;

/*
 * JACK callbacks
 */

double note=440;

int process(jack_nframes_t nframes, void *arg){
    // set when we are running a command and reset when it ends.
    // ONLY USED IN THIS THREAD.
    static bool isCmdRunning=false;
    
    jack_default_audio_sample_t *out = 
          (jack_default_audio_sample_t *)jack_port_get_buffer(output_port,
                                                              nframes);
    
    // check pending commands
    if(pthread_mutex_trylock(&lock)==0){
        // process pending command here. Currently the only command is
        // "bip"
        if(cmdPending==1){
            amp=1;
            isCmdRunning=true;
            cmdPending=0; // remove command
            note = (rand()%300)+200;
        } else {
            // note that if a command was received,
            // we keep the lock until the command completes
            pthread_mutex_unlock(&lock);
        }
        
    }
    
    
    // make noises
    for(int i=0;i<nframes;i++){
        sosc1.setFrequency(note*1.7);
        sosc2.setFrequency(note);
        sosc2.setPM(sosc1.get()*0.1);
        
        sosc1.update();
        sosc2.update();
        
        out[i] = sosc2.get()*amp*0.2;
        
        amp *= 0.9999;
        
        // detect command completion, and unlock the thread if
        // complete.
        if(isCmdRunning && amp<0.1){
            isCmdRunning=false;
            pthread_mutex_unlock(&lock);
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



// send commands to the process. Will block until the process thread
// has completed any command.
void cmd(int x){
    printf("Waiting for lock..\n");
    pthread_mutex_lock(&lock);
    cmdPending=x;
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
        cmd(1);
        sleep(1);
    }
    
    jack_client_close(client);
    exit(0);
}
