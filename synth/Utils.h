/**
 * @file Utils.h
 * @brief Utility gens
 *
 */

#ifndef __UTILS_H
#define __UTILS_H


/// 2 input constant mixer

class ConstMix : public Gen {
    double amp1,amp2;
public:
    ConstMix() : Gen("constmix") {
        addin("a",0);
        addin("b",1);
    }
    virtual ~ConstMix(){}
    
    virtual void update(int nframes){
        double *in1 = ins[0];
        double *in2 = ins[1];
        for(int i=0;i<nframes;i++){
            out[i]=in1[i]*amp1 + in2[i]*amp2;
        }
    }
    
    virtual bool setParam(const char *k,const char *v){
        if(!strcmp("ampa",k))amp1=atof(v);
        else if(!strcmp("ampb",k))amp2=atof(v);
        else return Gen::setParam(k,v);
        return true;
    }
};



#endif /* __UTILS_H */
