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
    static float *table;
    float phaseacc;
    float phase;
    float freq; // actually freq/samprate
    float pmamount; // amount of phase mod
    float fmamount; // amount of freq mod
    bool fixed; // fixed frequency, or multiple of key frequency
public:
    
    virtual ~SinOsc(){}
    SinOsc() : Gen("sin") {
        addin("amp",SINOSC_AMP);
        addin("pm",SINOSC_PM);
        addin("fm",SINOSC_FM);
        
        fixed=false;
        phaseacc=0.0f;
        pmamount=0.1f;
        fmamount=0.1f;
        setFrequency(1);
        if(!table){
            table = new float[SINETABLESIZE];
            for(int i=0;i<SINETABLESIZE;i++){
                float x = (i*2.0f*M_PI)/(float)SINETABLESIZE;
                table[i]=sin(x);
            }
        }
    }
    
    virtual bool setParam(const char *k,const char *v){
        if(!strcmp("freq",k))setFrequency(atof(v));
        else if(!strcmp("pm",k))setPM(atof(v));
        else if(!strcmp("fm",k))setFM(atof(v));
        else if(!strcmp("mode",k))fixed=!strcmp(v,"fixed");
        else return Gen::setParam(k,v);
        return true;
    }
    
    /// if fixed is true, set to an actual pitch, otherwise set
    /// to some multiple of key (i.e. note
    /// played) frequency (this is the default).
    
    void setFrequency(float f){
        freq = f;
    }
    
    void setPM(float m){
        pmamount = m*0.01f;
    }
    
    void setFM(float m){
        fmamount = m*0.01f; // should be really tiny
    }
    
    virtual void update(int nframes){
        float *amp = ins[SINOSC_AMP];
        float *pm = ins[SINOSC_PM];
        float *fm = ins[SINOSC_FM];
        
        extern float samprate,keyFreq;
        float f = freq;
        if(!fixed)f*=keyFreq;
        float step = f/samprate;
        
        for(int i=0;i<nframes;i++){
            float istep = step + (fm?fm[i]*fmamount:0.0f);
            phaseacc += istep;
            if(phaseacc>1.0f)phaseacc -= 1.0f;
            if(phaseacc>1.0f)phaseacc -= 1.0f;
            // add 1 to avoid it going negative when we add the PM
            float iphase = 1.0+phaseacc + (pm?pm[i]*pmamount:0.0);
            int x = iphase*SINETABLESIZE;
            x %= SINETABLESIZE;
            out[i]=table[x] * (amp?amp[i]:1.0);
//            printf("-- %f %f %f\n",ifreq,amp[i],out[i]);
        }
        scaleOut(nframes);
    }
};





#endif /* __SINOSC_H */
