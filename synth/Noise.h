/**
 * @file NoiseGens.h
 * @brief  Brief description of file.
 *
 */

#ifndef __NOISEGENS_H
#define __NOISEGENS_H

#include "gen.h"

//inputs
#define NOISE_AMP 0

class Noise : public Gen {
public:
    Noise() : Gen("noise"){
        addin("amp",0);
    }
    virtual ~Noise(){}
    virtual void update(int nframes){
        float *amp = ins[NOISE_AMP];
        for(int i=0;i<nframes;i++){
            double f = drand48();
            f -= 0.5;
            out[i]=((float)f)*(amp?amp[i]:1.0);
        }
        scaleOut(nframes);
    }
};
    

#endif /* __NOISEGENS_H */
