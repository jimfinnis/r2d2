/**
 * @file Env.h
 * @brief  Brief description of file.
 *
 */

#ifndef __ENV_H
#define __ENV_H

#include "ctype.h"

inline float lerp(float a,float b,float t){
    return (1.0f-t)*a + t*b;
}

// number of points in envelope
#define ENVPOINTS 10

/// env generator. Call setlev/settime to fill the array, then call prep().
/// Call reset to rewind the generator if required.

class Env : public Gen {
    double levels[ENVPOINTS]; // level at time t
    double times[ENVPOINTS];  // time before level t since last level
    double acctimes[ENVPOINTS];// time of each level (since start of env)
    int nlevs;
    
    // the next level we are waiting for
    int nextlev;
    // current time
    float time;
public:
    
    Env(){
        nlevs=0;
    }
    
    void reset(){
        time=0;
        nextlev=0;
        done=false;
    }
    
    virtual bool setParam(const char *k,const char *v){
        if(isdigit(k[1])){
            int lev = k[1]-'0';
            if(lev>=nlevs)nlevs=lev+1;
            if(k[0]=='t')
                times[lev]=atof(v);
            else if(k[0]=='l')
                levels[lev]=atof(v);
        }
        else return Gen::setParam(k,v);
    }
    
    // call after setlev/settime calls
    void prep(){
        // accumulate times
        acctimes[0]=times[0];
        for(int i=1;i<nlevs;i++){
            if(times[i]<0.001)times[i]=0.001;
            acctimes[i]=acctimes[i-1]+times[i];
        }
        reset();
    }
    
    virtual void update(int nframes){
        extern double samprate;
        double step = 1.0/samprate;
        
        for(int i=0;i<nframes;i++){
            time += step;
            if(time<acctimes[0])
                out[i]=levels[0];
            else if(nextlev<0){ // finished
                out[i]=levels[nlevs-1];
                done=true;
            } else {
                if(time>acctimes[nextlev]){
                    nextlev++;
                    if(nextlev==nlevs){
                        out[i]=levels[nlevs-1];
                        nextlev=-1;
                        continue;
                    }
                }
                out[i] = lerp(levels[nextlev-1],levels[nextlev],
                              (time-acctimes[nextlev-1])/times[nextlev]);
            }
        }
        scaleOut(nframes);
    }
};

#endif /* __ENV_H */
