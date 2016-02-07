/**
 * @file Utils.h
 * @brief Utility gens
 *
 */

#ifndef __UTILS_H
#define __UTILS_H

namespace sonicAESSynth {

/// 2 input constant mixer

class ConstMix : public Gen {
    float amp1,amp2;
public:
    ConstMix() : Gen("mix2") {
        addin("a",0);
        addin("b",1);
        addin("amp",2);
        amp1=0.5;amp2=0.5; // default equal mix
    }
    virtual ~ConstMix(){}
    
    virtual void update(int nframes){
        float *in1 = ins[0]?ins[0]:floatZeroes;
        float *in2 = ins[1]?ins[1]:floatZeroes;
        float *amp = ins[2]?ins[2]:floatOnes;
        for(int i=0;i<nframes;i++){
            out[i]=(in1[i]*amp1 + in2[i]*amp2)*amp[i];
        }
    }
    
    virtual bool setParam(const char *k,const char *v){
        if(!strcmp("ampa",k))amp1=atof(v);
        else if(!strcmp("ampb",k))amp2=atof(v);
        else return Gen::setParam(k,v);
        return true;
    }
};

/// Adder

class Add : public Gen {
    float amp1,amp2;
public:
    Add() : Gen("add") {
        addin("a",0);
        addin("b",1);
        addin("amp",2);
    }
    virtual ~Add(){}
    
    virtual void update(int nframes){
        float *in1 = ins[0]?ins[0]:floatZeroes;
        float *in2 = ins[1]?ins[1]:floatZeroes;
        float *amp = ins[2]?ins[2]:floatOnes;
        for(int i=0;i<nframes;i++){
            out[i]=(in1[i] + in2[i])*amp[i];
        }
    }
};

/// Multiplier

class Mul : public Gen {
public:
    Mul() : Gen("mul") {
        addin("a",0);
        addin("b",1);
        addin("amp",2);
    }
    virtual ~Mul(){}
    
    virtual void update(int nframes){
        float *in1 = ins[0]?ins[0]:floatZeroes;
        float *in2 = ins[1]?ins[1]:floatZeroes;
        float *amp = ins[2]?ins[2]:floatOnes;
        for(int i=0;i<nframes;i++){
            out[i]=(in1[i] * in2[i])*amp[i];
        }
    }
};

/// Constant

class Const : public Gen {
    float c;
public:
    Const() : Gen("const") {c=1;}
    virtual ~Const(){}
    
    virtual void update(int nframes){
        for(int i=0;i<nframes;i++){
            out[i]=c;
        }
    }
    virtual bool setParam(const char *k,const char *v){
        if(!strcmp("val",k))c=atof(v);
        else return Gen::setParam(k,v);
        return true;
    }
};

}

#endif /* __UTILS_H */
