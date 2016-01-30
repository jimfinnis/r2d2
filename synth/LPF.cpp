/**
 * @file LPF.cpp
 * @brief See LPF.h
 *
 */

#include <stdio.h>
#include "LPF.h"

static double Pi;

static BIQUAD protoCoef[FILTER_SECTIONS]=
{
    {1.0,0,0,1.0,0.765367,1.0},
    {1.0,0,0,1.0,1.847759,1.0}
};

void prewarp(
    double *a0, double *a1, double *a2,
    double fc, double fs)
{
    double wp;

    wp = 2.0 * fs * tan(Pi * fc / fs);

    *a2 = (*a2) / (wp * wp);
    *a1 = (*a1) / wp;
}

void bilinear(
    double a0, double a1, double a2,    /* numerator coefficients */
    double b0, double b1, double b2,    /* denominator coefficients */
    double *k,           /* overall gain factor */
    double fs,           /* sampling rate */
    float *coef         /* pointer to 4 iir coefficients */
)
{
    double ad, bd;

                 /* alpha (Numerator in s-domain) */
    ad = 4. * a2 * fs * fs + 2. * a1 * fs + a0;
                 /* beta (Denominator in s-domain) */
    bd = 4. * b2 * fs * fs + 2. * b1* fs + b0;

                 /* update gain constant for this section */
    *k *= ad/bd;

                 /* Denominator */
    *coef++ = (2. * b0 - 8. * b2 * fs * fs)
                           / bd;         /* beta1 */
    *coef++ = (4. * b2 * fs * fs - 2. * b1 * fs + b0)
                           / bd; /* beta2 */

                 /* Nominator */
    *coef++ = (2. * a0 - 8. * a2 * fs * fs)
                           / ad;         /* alpha1 */
    *coef = (4. * a2 * fs * fs - 2. * a1 * fs + a0)
                           / ad;   /* alpha2 */
}

void szxform(
    double *a0, double *a1, double *a2, /* numerator coefficients */
    double *b0, double *b1, double *b2, /* denominator coefficients */
    double fc,         /* Filter cutoff frequency */
    double fs,         /* sampling rate */
    double *k,         /* overall gain factor */
    float *coef)         /* pointer to 4 iir coefficients */
{
                 /* Calculate a1 and a2 and overwrite the original values */
        prewarp(a0, a1, a2, fc, fs);
        prewarp(b0, b1, b2, fc, fs);
        bilinear(*a0, *a1, *a2, *b0, *b1, *b2, k, fs, coef);
}

LPF::LPF() : Gen("lpf") {
    addin("in",0);
    addin("qm",1);
    addin("fm",2);
    
    qmod=0.1;
    fmod=1;
    
    Pi = 4.0 * atan(1.0); // could be done better...
    iir.length = FILTER_SECTIONS;
    iir.coef = (float *) calloc(4 * iir.length + 1, sizeof(float));
    if (!iir.coef)
        throw Exception("cannot allocate coef array in LPF");
    
    q2=q = 1.0; // resonance
    gain = 1.0; // overall gain
    fc2=fc = 8000.0; // cutoff
    reinit=true;
}



void LPF::init(){
    float *coef = iir.coef + 1;     /* Skip k, or gain */
    extern float samprate;
    double fs= samprate;
    double k = gain;
    for(unsigned int i=0;i<iir.length;i++){
        double a0 = protoCoef[i].a0;
        double a1 = protoCoef[i].a1;
        double a2 = protoCoef[i].a2;
        double b0 = protoCoef[i].b0;
        double b1 = protoCoef[i].b1/q2;
        double b2 = protoCoef[i].b2;
        szxform(&a0, &a1, &a2, &b0, &b1, &b2, fc2, fs, &k, coef);
        coef+=4;
    }
    iir.coef[0]=k;
    reinit=false;
}

void LPF::update(int nframes){
    unsigned int j;
    float *hist1_ptr,*hist2_ptr,*coef_ptr;
    float output,new_hist,history1,history2;
    
    /* allocate history array if different size than last call */
    
    if(!iir.history) {
        iir.history = (float *) calloc(2*iir.length,sizeof(float));
        if(!iir.history)
            throw Exception("Unable to allocate history array in LPF");
    }
    
    float *in = ins[0];
    float *qmods = ins[1]?ins[1]:floatZeroes;
    float *fmods = ins[2]?ins[2]:floatZeroes;
    if(reinit||ins[1]||ins[2])init();
    for(int i=0;i<nframes;i++){
        q2=q+qmod*qmods[i];
        fc2=fc+fmod*fmods[i];
        
        float input = *in++;
        
        coef_ptr = iir.coef;                /* coefficient pointer */

        hist1_ptr = iir.history;            /* first history */
        hist2_ptr = hist1_ptr + 1;           /* next history */

        /* 1st number of coefficients array is overall input scale factor,
         * or filter gain */
        output = input * (*coef_ptr++);

        for(j = 0 ; j < iir.length; j++)
        {
            history1 = *hist1_ptr;           /* history values */
            history2 = *hist2_ptr;

            output = output - history1 * (*coef_ptr++);
            new_hist = output - history2 * (*coef_ptr++);    /* poles */

            output = new_hist + history1 * (*coef_ptr++);
            output = output + history2 * (*coef_ptr++);      /* zeros */

            *hist2_ptr++ = *hist1_ptr;
            *hist1_ptr++ = new_hist;
            hist1_ptr++;
            hist2_ptr++;
        }
        out[i]=output;
    }
}


