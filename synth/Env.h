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
    float levels[ENVPOINTS]; // level at time t
    float times[ENVPOINTS];  // time before level t since last level
    float acctimes[ENVPOINTS];// time of each level (since start of env)
    int nlevs;
    
    // does this need prepping?
    bool prepped;
    
    // the next level we are waiting for
    int nextlev;
    // current time
    float time;
public:
    
    Env() : Gen("env"){
        nlevs=0;
        reset();
    }
    virtual ~Env(){}
    
    void reset(){
        time=0;
        nextlev=0;
        done=false;
        prepped=false;
    }
    
    // call after setlev/settime calls
    void prep(){
        // accumulate times
        times[0]=acctimes[0];
        for(int i=1;i<nlevs;i++){
            times[i] = acctimes[i]-acctimes[i-1];
            if(times[i]<0)throw BadTimeException(genname);
            if(times[i]<0.001)times[i]=0.001;
            acctimes[i]=acctimes[i-1]+times[i];
        }
        /*
           for(int i=0;i<nlevs;i++){
            printf("Level %d lev %f time %f acctime %f\n",i,
                   levels[i],times[i],acctimes[i]);
           }
         */
        prepped=true;
    }
    
    virtual bool setParam(const char *k,const char *v){
        // reset the prep
        prepped=false;
        if(isdigit(k[1])){
            int lev = k[1]-'0';
            if(lev>=nlevs)nlevs=lev+1;
            if(k[0]=='t')
                acctimes[lev]=atof(v);
            else if(k[0]=='l')
                levels[lev]=atof(v);
            return true;
        }
        else return Gen::setParam(k,v);
    }
    
    virtual void update(int nframes){
        extern float samprate;
        float step = 1.0/samprate;
        if(!prepped)prep();
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
//            printf("Level %d lev %f time %f acctime %f\n",i,
        }
        scaleOut(nframes);
    }
};

#endif /* __ENV_H */
