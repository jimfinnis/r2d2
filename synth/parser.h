/**
 * @file parser.h
 * @brief  Builds synths with float parameters from a string
 *
 */

#ifndef __PARSER_H
#define __PARSER_H

#include "synth.h"
#include "tokeniser.h"
#include "tokens.h"
#include "exceptions.h"
#include "synthdef.h"


namespace sonicAESSynth {


/// the parser may either set internal state (creating or
/// modifying synthdefs etc.) or it can add one of these
/// to the command chain.
/// These are commands to be sent to the audio thread to
/// play a note on a freshly instantiated synth. 
/// They are linked together - when a NoteCmd finishes playing,
/// the next one is triggered.

struct NoteCmd {
    virtual ~NoteCmd(){
        delete synth;
    }
    NoteCmd(Synth *s,float f){
        synth = s;
        freq = f;
        next = NULL;
    }
    
    
    Synth *synth; //!< is deleted with this structure
    float freq;
    NoteCmd *next;
};

class Stack {
    float stack[8];
public:
    int ct;
    Stack(){reset();}
    void reset(){ct=0;}
    float pop(){
        if(ct==0)
            throw StackUnderflowException();
        return stack[--ct];
    }
    void push(float f){
        if(ct==7)
            throw StackOverflowException();
        stack[ct++]=f;
    }
};
          

class Parser {
private:
    SynthDef *cursynth;
    GenDef *curgen;
    Stack stack;
    
    Tokeniser tok;
    std::string getnextident(){
        if(tok.getnext()!=T_IDENT)
            throw SyntaxException();
        return std::string(tok.getstring());
    }
    
    void expect(int t,std::string tokname){
        if(tok.getnext()!=t)
            throw UnexpectedException("'"+tokname+"'",
                                      ("'"+
                                      std::string(tok.getstring())+"'").c_str());
    }
        
    
    void assertsynth(){
        if(!cursynth)
            throw Exception("no synth set");
    }
    void assertgen(){
        if(!curgen)
            throw Exception("no gen set");
    }
    
    /// parse note sequences
    void parseNotes();
    /// parse envelope generator steps
    void parseEnv(GenDef *e);
public:
    Parser();
    /// parse a bunch of commands
    void parse(const char *buf);
};

}


#endif /* __PARSER_H */
