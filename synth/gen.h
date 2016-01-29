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
#include <map>

#include "exceptions.h"

#define MAXFRAMESIZE 1024
#define MAXINPUTS 4

class Gen {
public:
    /// structure for mapping from names to strings. Terminate
    /// with NULL name.
    struct InputMap {
        const char *name;
        int n;
    };
    
    std::map<std::string,int> inputMap;
    
    double out[MAXFRAMESIZE];
    double *ins[MAXINPUTS];
    double masterAmp;
    const char *name; // the type name
    std::string genname; // the unique name.
    
    virtual ~Gen(){}
    
    Gen(const char *n){
        name = n;
        done=false;
        isDoneMon=false;
        masterAmp=1;
        for(int i=0;i<MAXINPUTS;i++)
            ins[i]=NULL;
        for(int i=0;i<MAXFRAMESIZE;i++)
            out[i]=0;
    }
    
    /// set the unique name for this gen
    void setName(std::string n){
        genname = n;
    }
          
    
    /// add inputs using this, in the constructor
    void addin(std::string s,int n){
        inputMap[s]=n;
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
        std::map<std::string,int>::const_iterator i = 
              inputMap.find(std::string(s));
        if(i == inputMap.end())
            throw UnknownInputException(name,s);
        else
            return i->second;
    }
};



#endif /* __GEN_H */
