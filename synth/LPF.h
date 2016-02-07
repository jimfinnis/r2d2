/**
 * @file LPF.h
 * @brief  Resonant low pass filter code,
 * based on www.musicdsp.org/files/filters004.txt  
 * written by baltrax@hotmail.com (Zxform)
 *
 */

#ifndef __LPF_H
#define __LPF_H

#include <math.h>
#include "gen.h"

namespace sonicAESSynth {

/* FILTER INFORMATION STRUCTURE FOR FILTER ROUTINES */

struct FILTER {
    unsigned int length;       /* size of filter */
    float *history;            /* pointer to history in filter */
    float *coef;               /* pointer to coefficients of filter */
    
    FILTER(){
        history=NULL;
        coef=NULL;
    }
    ~FILTER(){
        if(history)free(history);
        if(coef)free(coef);
    }
};

#define FILTER_SECTIONS   2   /* 2 filter sections for 24 db/oct filter */

struct BIQUAD {
        double a0, a1, a2;       /* numerator coefficients */
        double b0, b1, b2;       /* denominator coefficients */
};

class LPF : public Gen {
    FILTER iir;
    void init(); //!< setup from parameters
    
    double fc; //!< cutoff
    double q; //!< resonance 1.0 < q < 1000
    double gain; //!< gain (default 1)
    
    // modulated versions
    double fc2,q2;
    // stored to check distance
    double prevfc2,prevq2;
    
    double qmod,fmod; // modulation amounts
    
public:
    LPF();
    ~LPF(){}
    virtual bool setParam(const char *key,const char *v){
        if(!strcmp("freq",key))fc=atof(v);
        else if(!strcmp("q",key))q=atof(v);
        else if(!strcmp("gain",key))gain=atof(v);
        else if(!strcmp("qmod",key))qmod=atof(v);
        else if(!strcmp("fmod",key))fmod=atof(v);
        else return Gen::setParam(key,v);
        return true;
    }
    
    virtual void update(int nframes);
};

}
#endif /* __LPF_H */
