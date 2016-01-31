//
//  WaveTableOsc.h
//
//  Created by Nigel Redmon on 5/15/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the wavetable oscillator and code,
//  read the series of articles by the author, starting here:
//  www.earlevel.com/main/2012/05/03/a-wavetable-oscillator—introduction/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//




#ifndef Test_WaveTableOsc_h
#define Test_WaveTableOsc_h

#include "gen.h"

#define doLinearInterp 1

typedef struct {
    double topFreq;
    int waveTableLen;
    float *waveTable;
} waveTable;

const int numWaveTableSlots = 32;

#define WAVEOSC_AMP 0
#define WAVEOSC_FM 1

class WaveTableOsc : public Gen {
protected:
    double phasor;      // phase accumulator
    double phaseInc;    // phase increment
    double phaseOfs;    // phase offset for PWM
    double freq;
    double fmamount;
    int recalcinterval; // recalculate freq every n samples (FM rate)
    
    bool fixed; // fixed frequency, or multiple of key frequency
    
    // list of wavetables
    int numWaveTables;
    waveTable waveTables[numWaveTableSlots];
    int addWaveTable(int len, float *waveTableIn, double topFreq);
    float makeWaveTable(int len, double *ar, double *ai, double scale, double topFreq);
    void fillTables(double *freqWaveRe, double *freqWaveIm, int numSamples);
    void reset();
    
public:
    WaveTableOsc(void);
    virtual ~WaveTableOsc(void);
    
    // INITIALIZERS
    
    void waveOsc(double *waveSamples, int tableLen);
    void sawOsc();
    void squareOsc();
    void triangleOsc();
    void sinOsc();// don't use, it's pointless :)
    
    virtual bool setParam(const char *k,const char *v){
        if(!strcmp("freq",k))setFrequency(atof(v));
        else if(!strcmp("fm",k))fmamount=atof(v);
        else if(!strcmp("recalc",k))recalcinterval=atoi(v);
        else if(!strcmp("type",k)){
            if(!strcmp("saw",v))
                sawOsc();
            else if(!strcmp("square",v))
                squareOsc();
            else if(!strcmp("triangle",v))
                triangleOsc();
            else 
                throw Exception("unsupported type in waveosc");
        }
        else if(!strcmp("mode",k))fixed=!strcmp(v,"fixed");
        else return Gen::setParam(k,v);
        return true;
    }
    
    void setFrequency(double inc);
    void setPhaseOffset(double offset);
    virtual void update(int nframes);
    
    
};


inline void WaveTableOsc::setFrequency(double f) {
    freq = f;
}

// note: if you don't keep this in the range of 0-1, you'll need to make changes elsewhere
inline void WaveTableOsc::setPhaseOffset(double offset) {
    phaseOfs = offset;
}

#endif
