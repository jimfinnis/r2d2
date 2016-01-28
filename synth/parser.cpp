/**
 * @file parser.cpp
 * @brief  Brief description of file.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "tokens.h"
#include "parser.h"

#include "SinOsc.h"
#include "Env.h"
#include "Noise.h"
double *SinOsc::table=NULL;


class SBeep : public Synth {
private:
    SinOsc sosc;
    Env env;
public:
    
    SBeep(Tokeniser *tok){
        float f = tok->getnextfloat();
        if(f<0)f=440.0; // error defaults to sane value
        
        env.addstage(0,0);
        env.addstage(0.1,1);
        env.addstage(0.1,0.7);
        env.addstage(0.2,0.6);
        env.addstage(0.5,0);
        env.prep();
        sosc.setFrequency(f);
        sosc.setin(SINOSC_AMP,&env);
        add(&sosc);// is the output
        add(&env,true);
    }
};

class SBeep2 : public Synth {
private:
    SinOsc sosc1,sosc1mod;
    SinOsc sosc2,sosc2mod;
    Noise noise;
    Env env1,env2;
    ConstMix mix;
public:
    
    SBeep2(Tokeniser *tok){
        float f = tok->getnextfloat();
        if(f<0)f=440.0; // error defaults to sane value
        
        sosc1.setin(SINOSC_AMP,&env1);
        sosc1.setin(SINOSC_PM,&sosc1mod);
        sosc1.setin(SINOSC_FM,&sosc2);
        sosc1.setPM(0.01);
        sosc1.setFM(0.1);
        
//        sosc2.setin(SINOSC_PM,&sosc2mod);
        sosc2.setin(SINOSC_AMP,&env2);
        
        sosc1.setFrequency(f);
        sosc2.setFrequency(10);
        sosc1mod.setFrequency(f*1.2);
        sosc2mod.setFrequency(f*1.9);
        
        env1.addstage(0,0);
        env1.addstage(0.1,1);
        env1.addstage(0.1,0.7);
        env1.addstage(0.2,0.6);
        env1.addstage(0.5,0);
        env1.prep();
        
        env2.addstage(0,1);
        env2.addstage(0.1,1);
        env2.addstage(0.2,0);
        env2.prep();
        
        noise.setin(NOISE_AMP,&env2);
        mix.setins(&sosc1,0.0,&noise,0.8);
        
        add(&mix);// is the output
        add(&sosc1);
        add(&sosc2);
        add(&sosc1mod);
        add(&sosc2mod);
        add(&env1,true);
        add(&env2);
        add(&noise);
    }
};




//////////////////////////////////////////////////////////////////

Parser::Parser(){
#if 0
    
    
    extern double samprate;
    samprate=100;
    Noise n;
    for(int i=0;i<120;i++){
        n.update(10);
        for(int j=0;j<10;j++){
            printf("%3d %f\n",i,n.out[j]);
        }
    }
    exit(0);
#endif
    tok.init();
    tok.setname("<in>");
    tok.settokens(tokens);
    tok.setcommentlinesequence("#");
}

Synth *Parser::parseSynth(){
    const char *s = tok.getstring();
    if(!strcmp("beep",s)){
        return new SBeep2(&tok);
    }
    return NULL;
}

Synth *Parser::parse(const char *buf){
    tok.reset(buf);
    
    Synth *head=NULL,*tail=NULL,*ss;
    tok.settrace(1);
    for(;;){
        switch(tok.getnext()){
        case T_END:
            return head;
        case T_SEMICOLON:
            break;
        case T_IDENT:
            ss = parseSynth();
            if(!ss)
                printf("Unknown synth: %s\n",tok.getstring());
            else {
                if(!head){head=tail=ss;}
                else {
                    tail->next = ss;
                    tail=ss;
                }
            }
            break;
        default:
            printf("Error in synth line\n");break;
        }
    }
}
