/**
 * @file parser.cpp
 * @brief  Brief description of file.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "tokens.h"
#include "parser.h"

#include "synthdef.h"



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
    }
    return NULL;
}

SynthDef *cursynth=NULL;
GenDef *curdef=NULL;

Synth *Parser::parse(const char *buf){
    std::string a,b,c;
          
    tok.reset(buf);
    Synth *head=NULL,*tail=NULL,*ss;
    tok.settrace(1);
    for(;;){
        switch(tok.getnext()){
        case T_NEWSYNTH:
            a = getnextident();
            cursynth = new SynthDef();
            synths[a]=cursynth;
            break;
        case T_MODSYNTH:
            a = getnextident();
            cursynth = synths[a];
            break;
        case T_G:
            a = getnextident();
            b = getnextident();
            cursynth->add(a,b);
            break;
        case T_P:
            for(;;){
                a = getnextident();
                b = getnextident();
                if(tok.getnext()==T_SEMICOLON)
                    break;
                cursynth->setParam(a,b);
            }
            break;
        case T_MINUS:
            a = getnextident();
            b = getnextident();
            c = getnextident();
            cursynth->addlink(a.c_str(),b.c_str(),c.c_str());
            break;
        default:
            printf("Error in synth line\n");break;
        }
    }
}
