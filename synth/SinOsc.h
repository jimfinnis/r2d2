/**
 * @file SinOsc.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SINOSC_H
#define __SINOSC_H

#include "gen.h"

#define SINETABLESIZE 2048

// inputs 
#define SINOSC_AMP 0
#define SINOSC_PM 1
#define SINOSC_FM 2

class SinOsc : public Gen {
private:
    static double *table;
    double phaseacc;
    double phase;
    double freq; // actually freq/samprate
    double pmamount; // amount of phase mod
    double fmamount; // amount of freq mod
    
public:
    
    SinOsc(){
        phaseacc=0;
        pmamount=0.1;
        fmamount=0.1;
        setFrequency(440);
        if(!table){
            table = new double[SINETABLESIZE];
            for(int i=0;i<SINETABLESIZE;i++){
                double x = (i*2*M_PI)/(double)SINETABLESIZE;
                table[i]=sin(x);
            }
        }
    }
    
    virtual bool setParam(const char *k,const char *v){
        if(!strcmp("freq",k))setFrequency(atof(v));
        if(!strcmp("pm",k))setPM(atof(v));
        if(!strcmp("fm",k))setFM(atof(v));
        else return Gen::setParam(k,v);
    }
    void setFrequency(double f){
        extern double samprate;
        freq = f/samprate;
    }
    
    void setPM(double m){
        pmamount = m;
    }
    
    void setFM(double m){
        fmamount = m*0.01; // should be really tiny
    }
    
    virtual void update(int nframes){
        double *amp = ins[SINOSC_AMP];
        double *pm = ins[SINOSC_PM];
        double *fm = ins[SINOSC_FM];
        
        for(int i=0;i<nframes;i++){
            
            double ifreq = freq + (fm?fm[i]*fmamount:0.0);
            phaseacc += ifreq;
            if(phaseacc>1.0)phaseacc -= 1.0;
            if(phaseacc>1.0)phaseacc -= 1.0;
            double iphase = phaseacc + (pm?pm[i]*pmamount:0.0);
            int x = iphase*SINETABLESIZE;
            x %= SINETABLESIZE;
            out[i]=table[x] * (amp?amp[i]:1.0);
        }
        scaleOut(nframes);
    }
};





#endif /* __SINOSC_H */
