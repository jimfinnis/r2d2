/**
 * @file gen.h
 * @brief  Interface for elements of a synth
 *
 */

#ifndef __GEN_H
#define __GEN_H

#include <math.h>
#include <stdlib.h>
#include <string.h>

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
    
    /// set a named parameter inside the gen, done by the SynthDef.
    /// Returns false if there is no such param, which is a bit duff -
    /// I'd rather do the checking at parse time, not build time.
    /// Overriden to do more in each gen.
    virtual bool setParam(const char *k,const char *v){
        if(!strcmp(k,"amp")){
            masterAmp=atof(v);
            return true;
        }
        else return false;
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
    
    int getInputByName(const char *s){
        return 0;
    }
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
    
    virtual bool setParam(const char *k,const char *v){
        if(!strcmp("amp1",k))amp1=atof(v);
        else if(!strcmp("amp1",k))amp2=atof(v);
        else return Gen::setParam(k,v);
        return true;
    }
};
          
    



#endif /* __GEN_H */
