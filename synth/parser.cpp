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
double *SinOsc::table=NULL;


class SBeep : public Synth {
private:
    SinOsc sosc1,sosc2;
    float amp;
public:
    
    SBeep(Tokeniser *tok){
        float f = tok->getnextfloat();
        if(f<0)f=440.0; // error defaults to sane value
        sosc1.setFrequency(f*1.7);
        sosc2.setFrequency(f);
        amp=1;
    }
    
    virtual float update(){
        sosc2.setPM(sosc1.update()*0.1);
        amp*=0.999;
        if(amp<0.01)done=true;
        return sosc2.update()*amp;
    }
};

class SBeep2 : public Synth {
private:
    SinOsc sosc;
    Env env;
public:
    
    SBeep2(Tokeniser *tok){
        float f = tok->getnextfloat();
        if(f<0)f=440.0; // error defaults to sane value
        sosc.setFrequency(f);
        env.set(0.01,0.01,0.1,0.2, 0.8);
    }
    
    virtual float update(){
        if(env.done)done=true;
        return sosc.update() * env.update();
    }
};




//////////////////////////////////////////////////////////////////

Parser::Parser(){
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
