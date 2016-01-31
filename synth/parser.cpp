/**
 * @file parser.cpp
 * @brief  Brief description of file.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "tokens.h"
#include "parser.h"
#include "cmdlist.h"

#include <sstream>


//////////////////////////////////////////////////////////////////

Parser::Parser(){
#if 0
    extern float samprate;
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
    cursynth=NULL;
    curgen=NULL;
    tok.init();
    tok.setname("<in>");
    tok.settokens(tokens);
    tok.setcommentlinesequence("#");
}

std::string f2string(float t){
    std::ostringstream s;
    s << t;
    return s.str();
}

void Parser::parseNotes(){
    float a,b;
    for(;;){
        switch(tok.getnext()){
        case T_INT:case T_FLOAT:
            stack.push(tok.getfloat());
            break;
        case T_DOT:
            printf("%f\n",stack.pop());
            break;
        case T_END:
            return;
        case T_SEMICOLON:
        case T_COMMA:
            {
                extern CmdList cmds;
                float f = stack.pop();
                Synth *s = cursynth->build();
                NoteCmd *cmd = new NoteCmd(s,f);
                cmds.add(cmd);
            }
            break;
        case T_PLUS:
            stack.push(stack.pop()+stack.pop());
            break;
        case T_MUL:
            stack.push(stack.pop()*stack.pop());
            break;
        case T_DIV:
            b = stack.pop();
            a = stack.pop();
            stack.push(a/b);
            break;
        case T_MINUS:
            b = stack.pop();
            a = stack.pop();
            stack.push(a-b);
            break;
        case T_MOD:
            b = stack.pop();
            a = stack.pop();
            stack.push(fmodf(a,b));
            break;
        case T_DUP:
            a = stack.pop();
            stack.push(a);
            stack.push(a);
            break;
        case T_RAND: // a b rand -> rand in [a,b)
            b = stack.pop();
            a = stack.pop();
            a = drand48()*(b-a)+a;
            stack.push(a);
            break;
        default:
            throw NoteException(tok.getstring());
        }
    }
}

void Parser::parse(const char *buf){
    std::string a,b,c;
    int t;
    std::string line = buf;
    tok.reset(buf);
    
    
    try {
        for(;;){
            switch(tok.getnext()){
            case T_END:
                return;
            case T_COLON:// note sequence
                parseNotes();
                break;
            case T_NEWSYNTH:
                a = getnextident();
                cursynth = new SynthDef();
                synths[a]=cursynth;
                break;
            case T_S:
                a = getnextident();
                cursynth = synths[a];
                break;
            case T_G:
                assertsynth();
                a = getnextident();
                b = getnextident();
                curgen = cursynth->add(a,b);
                break;
            case T_M:
                assertsynth();assertgen();
                a = getnextident();
                curgen = cursynth->findGen(a);
                break;
            case T_P:
                assertsynth();assertgen();
                for(;;){
                    a = getnextident();
                    switch(tok.getnext()){
                    case T_IDENT:
                    case T_INT:
                    case T_FLOAT:
                        break;
                    default:
                        printf("%d\n",tok.gettoken().i);
                        throw UnexpectedException("ident or number",tok.getstring());
                    }
                    b = std::string(tok.getstring());
                    curgen->setParam(a,b);
                    t = tok.getnext();
                    if(t==T_SEMICOLON || t==T_END)
                        break;
                    else if(t!=T_COMMA)
                        throw SyntaxException();
                }
                break;
            case T_IDENT:
                assertsynth();
                a = std::string(tok.getstring());
                expect(T_ARROW,"->");
                b = getnextident();
                expect(T_COLON,":");
                c = getnextident();
                cursynth->addlink(a.c_str(),b.c_str(),c.c_str());
                break;
            case T_DONE:
                assertgen();
                curgen->setDoneMon();
                break;
            case T_OUT:
                assertgen();
                curgen->out=true;
                break;
                // special syntax to make an Env
            case T_ENV:
                assertsynth();
                a = getnextident();
                curgen = cursynth->add("env",a);
                {
                    char lbuf[]="lx",tbuf[]="tx";
                    for(int ct=0;ct<10;ct++){
                        lbuf[1]='0'+ct;
                        tbuf[1]='0'+ct;
                        float t = tok.getnextfloat();
                        if(tok.iserror())
                            throw UnexpectedException("time",tok.getstring());
                        float l = tok.getnextfloat();
                        if(tok.iserror())
                            throw UnexpectedException("level",tok.getstring());
                        curgen->setParam(tbuf,f2string(t));
                        curgen->setParam(lbuf,f2string(l));
                        
                        t = tok.getnext();
                        if(t==T_SEMICOLON || t==T_END)
                            break;
                        else if(t!=T_COMMA)
                            throw SyntaxException();
                    }
                }
                break;
            default:
                throw SyntaxException();
            }
        }
    } catch (Exception e) {
        printf("Exception in parsing: %s\n",e.what());
        printf("Line: %s\n",line.c_str());
        printf("Token: %s\n",tok.getstring());
        exit(1);
    }
}
