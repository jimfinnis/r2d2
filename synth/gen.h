/**
 * @file gen.h
 * @brief  Interface for elements of a synth
 *
 */

#ifndef __GEN_H
#define __GEN_H

#include <math.h>
#include <stdlib.h>
#define MAXFRAMESIZE 1024
#define MAXINPUTS 4

class Gen {
public:
    double out[MAXFRAMESIZE];
    double *ins[MAXINPUTS];
    double masterAmp;
    
    virtual ~Gen(){}
    
    Gen(){
        done=false;
        isDoneMon=false;
        masterAmp=1;
        for(int i=0;i<MAXINPUTS;i++)
            ins[i]=NULL;
        for(int i=0;i<MAXFRAMESIZE;i++)
            out[i]=0;
    }
    
    void setAmp(double a){
        masterAmp = a;
    }
    
    void scaleOut(int nframes){
        for(int i=0;i<nframes;i++)
            out[i]=masterAmp*out[i];
    }
              
    
    // set input N to be the output of gen G
    void setin(int n,Gen *g){
        ins[n] = g->out;
    }
    
    // assumes that any inputs are filled with data, and writes
    // to the output buffer
    virtual void update(int nframes) = 0;
    
    // links gens inside a synth; all linked this way will have
    // update called by synth update.
    Gen *next;
    // true if the gen has completed (for a triggerable, typically
    // an Env).
    bool done;
    // true if the gen is a done monitor (see synth)
    bool isDoneMon;
};


// some utility gens

class ConstMix : public Gen {
    double amp1,amp2;
public:
    virtual void update(int nframes){
        double *in1 = ins[0];
        double *in2 = ins[1];
        for(int i=0;i<nframes;i++){
            out[i]=in1[i]*amp1 + in2[i]*amp2;
        }
    }
    
    void setins(Gen *g1,double a1,Gen *g2,double a2){
        setin(0,g1); amp1=a1;
        setin(1,g2); amp2=a2;
    }
};
          
    



#endif /* __GEN_H */
