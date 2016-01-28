//
//  WaveTableOsc.cpp
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

#include "WaveTableOsc.h"


WaveTableOsc::WaveTableOsc(void) {
    phasor = 0.0;
    phaseInc = 0.0;
    phaseOfs = 0.5;
    numWaveTables = 0;
    for (int idx = 0; idx < numWaveTableSlots; idx++) {
        waveTables[idx].topFreq = 0;
        waveTables[idx].waveTableLen = 0;
        waveTables[idx].waveTable = 0;
    }
}


WaveTableOsc::~WaveTableOsc(void) {
    for (int idx = 0; idx < numWaveTableSlots; idx++) {
        float *temp = waveTables[idx].waveTable;
        if (temp != 0)
            delete [] temp;
    }
}


//
// addWaveTable
//
// add wavetables in order of lowest frequency to highest
// topFreq is the highest frequency supported by a wavetable
// wavetables within an oscillator can be different lengths
//
// returns 0 upon success, or the number of wavetables if no more room is available
//
int WaveTableOsc::addWaveTable(int len, float *waveTableIn, double topFreq) {
    if (this->numWaveTables < numWaveTableSlots) {
        float *waveTable = this->waveTables[this->numWaveTables].waveTable = new float[len];
        this->waveTables[this->numWaveTables].waveTableLen = len;
        this->waveTables[this->numWaveTables].topFreq = topFreq;
        ++this->numWaveTables;
        
        // fill in wave
        for (long idx = 0; idx < len; idx++)
            waveTable[idx] = waveTableIn[idx];
        
        return 0;
    }
    return this->numWaveTables;
}


//
// update
//
void WaveTableOsc::update(int nframes){
    
    for(int i=0;i<nframes;i++){
        // grab the appropriate wavetable
        int waveTableIdx = 0;
        while ((phaseInc >= waveTables[waveTableIdx].topFreq) && (waveTableIdx < (numWaveTables - 1))) {
            ++waveTableIdx;
        }
        waveTable *waveTable = &waveTables[waveTableIdx];
        
#if !doLinearInterp
        // truncate
        out[i]=  waveTable->waveTable[int(phasor * waveTable->waveTableLen)];
#else
        // linear interpolation
        double temp = phasor * waveTable->waveTableLen;
        int intPart = temp;
        double fracPart = temp - intPart;
        float samp0 = waveTable->waveTable[intPart];
        if (++intPart >= waveTable->waveTableLen)
            intPart = 0;
        float samp1 = waveTable->waveTable[intPart];
        
        out[i] = samp0 + (samp1 - samp0) * fracPart;
#endif
        
        phasor += phaseInc;
        
        if (phasor >= 1.0)
            phasor -= 1.0;
    }
    scaleOut(nframes);
}

//
// fft
//
// I grabbed (and slightly modified) this Rabiner & Gold translation...
//
// (could modify for real data, could use a template version, blah blah--just keeping it short)
//
static void fft(int N, double *ar, double *ai)
/*
   in-place complex fft
   
   After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)
   
   program adapted from FORTRAN 
   by K. Steiglitz  (ken@princeton.edu)
   Computer Science Dept. 
   Princeton University 08544          */
{    
    int i, j, k, L;            /* indexes */
    int M, TEMP, LE, LE1, ip;  /* M = log N */
    int NV2, NM1;
    double t;               /* temp */
    double Ur, Ui, Wr, Wi, Tr, Ti;
    double Ur_old;
    
    // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
    
    NV2 = N >> 1;
    NM1 = N - 1;
    TEMP = N; /* get M = log N */
    M = 0;
    while (TEMP >>= 1) ++M;
    
    /* shuffle */
    j = 1;
    for (i = 1; i <= NM1; i++) {
        if(i<j) {             /* swap a[i] and a[j] */
            t = ar[j-1];     
            ar[j-1] = ar[i-1];
            ar[i-1] = t;
            t = ai[j-1];
            ai[j-1] = ai[i-1];
            ai[i-1] = t;
        }
        
        k = NV2;             /* bit-reversed counter */
        while(k < j) {
            j -= k;
            k /= 2;
        }
        
        j += k;
    }
    
    LE = 1.;
    for (L = 1; L <= M; L++) {            // stage L
        LE1 = LE;                         // (LE1 = LE/2) 
        LE *= 2;                          // (LE = 2^L)
        Ur = 1.0;
        Ui = 0.; 
        Wr = cos(M_PI/(float)LE1);
        Wi = -sin(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here
        for (j = 1; j <= LE1; j++) {
            for (i = j; i <= N; i += LE) { // butterfly
                ip = i+LE1;
                Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                ar[ip-1] = ar[i-1] - Tr;
                ai[ip-1] = ai[i-1] - Ti;
                ar[i-1]  = ar[i-1] + Tr;
                ai[i-1]  = ai[i-1] + Ti;
            }
            Ur_old = Ur;
            Ur = Ur_old * Wr - Ui * Wi;
            Ui = Ur_old * Wi + Ui * Wr;
        }
    }
}


//
// if scale is 0, auto-scales
// returns scaling factor (0.0 if failure), and wavetable in ai array
//
float WaveTableOsc::makeWaveTable( int len, double *ar, double *ai, double scale, double topFreq) {
    fft(len, ar, ai);
    
    if (scale == 0.0) {
        // calc normal
        double max = 0;
        for (int idx = 0; idx < len; idx++) {
            double temp = fabs(ai[idx]);
            if (max < temp)
                max = temp;
        }
        scale = 1.0 / max * .999;        
    }
    
    // normalize
    float wave[len];
    for (int idx = 0; idx < len; idx++)
        wave[idx] = ai[idx] * scale;
    
    if (addWaveTable(len, wave, topFreq))
        scale = 0.0;
    
    return scale;
}

void WaveTableOsc::fillTables(double *freqWaveRe, double *freqWaveIm, int numSamples) {
    int idx;
    
    // zero DC offset and Nyquist
    freqWaveRe[0] = freqWaveIm[0] = 0.0;
    freqWaveRe[numSamples >> 1] = freqWaveIm[numSamples >> 1] = 0.0;
    
    // determine maxHarmonic, the highest non-zero harmonic in the wave
    int maxHarmonic = numSamples >> 1;
    const double minVal = 0.000001; // -120 dB
    while ((fabs(freqWaveRe[maxHarmonic]) + fabs(freqWaveIm[maxHarmonic]) < minVal)
           && maxHarmonic) --maxHarmonic;
    
    // calculate topFreq for the initial wavetable
    // maximum non-aliasing playback rate is 1 / (2 * maxHarmonic), but we allow
    // aliasing up to the point where the aliased harmonic would meet the next
    // octave table, which is an additional 1/3
    double topFreq = 2.0 / 3.0 / maxHarmonic;
    
    // for subsquent tables, double topFreq and remove upper half of harmonics
    double *ar = new double [numSamples];
    double *ai = new double [numSamples];
    double scale = 0.0;
    while (maxHarmonic) {
        // fill the table in with the needed harmonics
        for (idx = 0; idx < numSamples; idx++)
            ar[idx] = ai[idx] = 0.0;
        for (idx = 1; idx <= maxHarmonic; idx++) {
            ar[idx] = freqWaveRe[idx];
            ai[idx] = freqWaveIm[idx];
            ar[numSamples - idx] = freqWaveRe[numSamples - idx];
            ai[numSamples - idx] = freqWaveIm[numSamples - idx];
        }
        
        // make the wavetable
        scale = makeWaveTable(numSamples, ar, ai, scale, topFreq);
        
        // prepare for next table
        topFreq *= 2;
        maxHarmonic >>= 1;
    }
    // pretty sure these aren't needed now JCF
    delete [] ar;
    delete [] ai;
}

void WaveTableOsc::sawOsc(void) {
    int tableLen = 2048;    // to give full bandwidth from 20 Hz
    int idx;
    double *freqWaveRe = new double [tableLen];
    double *freqWaveIm = new double [tableLen];
    
    // make a sawtooth
    for (idx = 0; idx < tableLen; idx++) {
        freqWaveIm[idx] = 0.0;
    }
    freqWaveRe[0] = freqWaveRe[tableLen >> 1] = 0.0;
    for (idx = 1; idx < (tableLen >> 1); idx++) {
        freqWaveRe[idx] = 1.0 / idx;                    // sawtooth spectrum
        freqWaveRe[tableLen - idx] = -freqWaveRe[idx];  // mirror
    }
    
    // build a wavetable oscillator
    fillTables(freqWaveRe, freqWaveIm, tableLen);
    
    delete [] freqWaveRe;
    delete [] freqWaveIm;
}

void WaveTableOsc::sinOsc(void) {
    int tableLen = 2048;    // to give full bandwidth from 20 Hz
    double *freqWaveRe = new double [tableLen];
    double *freqWaveIm = new double [tableLen];
    
    for(int i=0;i<tableLen;i++){
        freqWaveRe[i] = freqWaveIm[i]=0;
    }
    freqWaveRe[1]=1;
    freqWaveRe[3]=0.1;
    
    // build a wavetable oscillator
    fillTables(freqWaveRe, freqWaveIm, tableLen);
    
    delete [] freqWaveRe;
    delete [] freqWaveIm;
}

void WaveTableOsc::waveOsc(double *waveSamples, int tableLen) {
    int idx;
    double *freqWaveRe = new double [tableLen];
    double *freqWaveIm = new double [tableLen];
    
    // convert to frequency domain
    for (idx = 0; idx < tableLen; idx++) {
        freqWaveIm[idx] = waveSamples[idx];
        freqWaveRe[idx] = 0.0;
    }
    fft(tableLen, freqWaveRe, freqWaveIm);
    
    // build the wavetable oscillator
    fillTables(freqWaveRe, freqWaveIm, tableLen);
    
    delete [] freqWaveRe;
    delete [] freqWaveIm;
}
