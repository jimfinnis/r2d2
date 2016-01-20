/**
 * @file Env.h
 * @brief  Brief description of file.
 *
 */

#ifndef __ENV_H
#define __ENV_H

inline float lerp(float a,float b,float t){
    return (1.0f-t)*a + t*b;
}

class Env : public Gen {
    float a,d,s,r; // these are all times, there is no gate
    float sl; // sustain level
    float time;
public:
    bool done;
    void set(float _a,float _d,float _s,float _r,float _sl){
        time=0;
        done=false;
        
        a=_a;
        if(a<0.001)a=0.001;
        d=_d;
        if(d<0.001)d=0.001;
        s=_s;
        if(s<0.001)s=0.001;
        r=_r;
        if(r<0.001)r=0.001;
        sl=_sl;
    }
    
    virtual float update(){
        extern double samprate;
        float t=time;
        time += 1.0/samprate;
        if(t<a){
            t = t/a;
        } else if(t<(a+d)){
            t = (t-a)/d;
            t = lerp(1,sl,t);
        } else if(t<(a+d+s)){
            t = sl;
        } else if(t<(a+d+s+r)){
            t = (t-(a+d+s))/r;
            t = lerp(sl,0,t);
        } else {
            t=0; done=true;
        }
        return t;
    }
};

#endif /* __ENV_H */
