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
#include "cmdlist.h"

jack_port_t *output_port;

/// table of midi note "frequencies" adjusted by sample rate
jack_default_audio_sample_t note_frqs[128]; 

/// sample rate 
double samprate = 0;
/// frequency of note being played
double keyFreq = 440.0;

/// various other globals
jack_default_audio_sample_t amp=0.0;


/// the command list 
CmdList cmds;


/*
 * JACK callbacks
 */

int process(jack_nframes_t nframes, void *arg){
    /// the actual synth currently running
    static NoteCmd *curcmd=NULL;
    
    jack_default_audio_sample_t *out = 
          (jack_default_audio_sample_t *)jack_port_get_buffer(output_port,
                                                              nframes);
    // make noises
    if(curcmd){
        curcmd->synth->update(nframes);
        double *outbuf = curcmd->synth->getout();
        for(jack_nframes_t i=0;i<nframes;i++){
            out[i] = outbuf[i];
        }
    } else {
        for(jack_nframes_t i=0;i<nframes;i++){
            out[i]=0;
        }
    }
    
    // detect command completion and move on to the next one,
    // which will delete the old one.
    
    if(!curcmd || curcmd->synth->done){
        if(curcmd)delete curcmd;
        curcmd = cmds.next();
        if(curcmd)
            keyFreq = curcmd->freq;
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

int main(int argc,char *argv[]){
    jack_client_t *client;
    
    
    // start JACK
    
    if (!(client = jack_client_open("r2d2", JackNullOption, NULL))){
        fprintf(stderr, "jack server not running?\n");
        return 1;
    }
    // get the real sampling rate
    samprate = (double)jack_get_sample_rate(client);
    
    
    // read initial commands. This has to happen after JACK
    // is initialised so we get the sampling rate.
    
    FILE *a = fopen("synthrc","r");
    if(a){
        while(!feof(a)){
            char buf[1024];
            char *bb = fgets(buf,1024,a);
            if(bb)parser.parse(bb);
        }
        fclose(a);
    }
/*    
    NoteCmd *c = cmds.next();
    for(int i=0;i<100;i++){
        c->synth->update(100);
        double *outbuf = c->synth->getout();
        for(int i=0;i<100;i++){
            printf("%f\n",outbuf[i]);
        }
    }
    exit(1);
*/    
    
    // set callbacks
    jack_set_process_callback(client, process, 0);
    jack_set_sample_rate_callback(client, srate, 0);
    jack_on_shutdown(client, jack_shutdown, 0);
    
    
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
        usleep(0.01);
        if(cmds.empty()){
            char buf[128];
            sprintf(buf,"m sin1 p pm %d,amp %f; m e p t3 %f;+%d;",
                    rand()%30,
                    drand48()*0.3 + 0.1,
                    drand48()*0.1+0.1,
                    200+(rand()%600));
            parser.parse(buf);
        }
    }
    
    while(1){
        char buf[1024];
        fputs(">> ",stdout);fflush(stdout);
        char *bb = fgets(buf,1024,stdin);
        if(bb)parser.parse(bb);
    }
    jack_client_close(client);
    exit(0);
}
