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
    bool fixed; // fixed frequency, or multiple of key frequency
public:
    
    virtual ~SinOsc(){}
    SinOsc() : Gen("sin") {
        addin("amp",SINOSC_AMP);
        addin("pm",SINOSC_PM);
        addin("fm",SINOSC_FM);
        
        fixed=false;
        phaseacc=0;
        pmamount=0.1;
        fmamount=0.1;
        setFrequency(1);
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
        else if(!strcmp("pm",k))setPM(atof(v));
        else if(!strcmp("fm",k))setFM(atof(v));
        else if(!strcmp("mode",k))fixed=!strcmp(v,"fixed");
        else return Gen::setParam(k,v);
        return true;
    }
    
    /// if fixed is true, set to an actual pitch, otherwise set
    /// to some multiple of key (i.e. note
    /// played) frequency (this is the default).
    
    void setFrequency(double f){
        freq = f;
    }
    
    void setPM(double m){
        pmamount = m*0.01;
    }
    
    void setFM(double m){
        fmamount = m*0.01; // should be really tiny
    }
    
    virtual void update(int nframes){
        double *amp = ins[SINOSC_AMP];
        double *pm = ins[SINOSC_PM];
        double *fm = ins[SINOSC_FM];
        
        extern double samprate,keyFreq;
        double f = freq;
        if(!fixed)f*=keyFreq;
        double step = f/samprate;
        
        for(int i=0;i<nframes;i++){
            double istep = step + (fm?fm[i]*fmamount:0.0);
            phaseacc += istep;
            if(phaseacc>1.0)phaseacc -= 1.0;
            if(phaseacc>1.0)phaseacc -= 1.0;
            double iphase = phaseacc + (pm?pm[i]*pmamount:0.0);
            int x = iphase*SINETABLESIZE;
            x %= SINETABLESIZE;
            out[i]=table[x] * (amp?amp[i]:1.0);
//            printf("-- %f %f %f\n",ifreq,amp[i],out[i]);
        }
        scaleOut(nframes);
    }
};





#endif /* __SINOSC_H */
