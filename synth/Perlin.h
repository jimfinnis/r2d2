/**
 * @file Perlin.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PERLIN_H
#define __PERLIN_H

#include "gen.h"

namespace sonicAESSynth {

class PerlinNoise
{
public:
    void init(
              float persistence, //!< persistence value - 0.25 is calm, 1 is spiky!
              int octaves   //!< number of octaves
              )
    {
        mPersistence = persistence;
        mOctaves = octaves;
    }
    
    /// get a noise value for a given x
    
    
    
    float get(float x)
    {
        float total = 0;
        
        float freq = 1;
        float amp = 1;
        for(int i=0;i<mOctaves;i++)
        {
            total += interpolatedNoise(i,x*freq) * amp;
            
            freq *= 2;
            amp *= mPersistence;
        }
        
        return total;
    }
    
private:
    
    
    /// noise functions
    
    inline float noise(int n, //!< function number
                       int x //!< argument
                       )
    {
        x = (x<<13)^x;
        x += n*31;
        return (1.0f - ((x*(x*x*15731+789221)+1376312589)&0x7fffffff)/1073741824.0f);
    }
    
    
    inline float smoothedNoise(int n,float x)
    {
        return noise(n,(int)x)/2 + noise(n,(int)x-1)/4 + noise(n,(int)x+1)/4;
    }
    
    inline float interpolatedNoise(int n,float x)
    {
        int inx = (int)x;
        float fracx = x - inx;
        float v1 = smoothedNoise(n,inx);
        float v2 = smoothedNoise(n,inx+1);
        
        return interpolate(v1,v2,fracx);
    }
    
    inline float interpolate(float a,float b,float x)
    {
        float ft = x * 3.1415927f;
        float f = (1-cosf(ft))*0.5f;
        return a*(1-f)+b*f;
    }
    
    int mOctaves;
    float mPersistence;
    
};

#define PERLIN_AMP 0

class Perlin : public Gen {
    PerlinNoise n;
    float phaseacc;
    float freq;
public:
    Perlin() : Gen("perlin"){
        n.init(0.24,1);
        freq=1;
        phaseacc=0;
        addin("amp",PERLIN_AMP);
    }
    virtual ~Perlin(){}
    
    virtual bool setParam(const char *k,const char *v){
        if(!strcmp("freq",k))freq=atof(v);
        else return Gen::setParam(k,v);
        return true;
    }
    
    inline float compress(float f){
        if(f>1)return 1;
        if(f<-1)return -1;
        else return f;
    }
    
    virtual void update(int nframes){
        extern float samprate;
        float *amp = ins[NOISE_AMP];
        float step = freq/samprate;
        
        for(int i=0;i<nframes;i++){
            phaseacc += step;
            float f = n.get(phaseacc);
            f = compress(f);
            out[i]=((float)f)*(amp?amp[i]:1.0);
        }
        scaleOut(nframes);
    }
};

}
#endif /* __PERLIN_H */
