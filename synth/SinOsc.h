/**
 * @file SinOsc.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SINOSC_H
#define __SINOSC_H

#include "gen.h"

#define SINETABLESIZE 2048

class SinOsc : public Gen {
private:
    static double *table;
    double phaseacc;
    double phase;
    double freq; // actually freq/samprate
    double phasemod;
public:
    
    SinOsc(){
        phaseacc=0;
        phasemod=0;
        setFrequency(440);
        if(!table){
            table = new double[SINETABLESIZE];
            for(int i=0;i<SINETABLESIZE;i++){
                double x = (i*2*M_PI)/(double)SINETABLESIZE;
                table[i]=sin(x);
            }
        }
    }
    
    void setFrequency(double f){
        extern double samprate;
        freq = f/samprate;
    }
    
    // set phase modulation value to be added to phase at
    // next update
    void setPM(double f){
        phasemod = f;
    }
    
    virtual float update() {
        phaseacc += freq;
        phase = phaseacc + phasemod;
        if(phase>1.0)phase -= 1.0;
        if(phase>1.0)phase -= 1.0;
            
        int x = phase*SINETABLESIZE;
        x %= SINETABLESIZE;
        return table[x];
    }
};
    
    
    
        

#endif /* __SINOSC_H */
