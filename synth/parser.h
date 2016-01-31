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

class Parser {
private:
    SynthDef *cursynth;
    GenDef *curgen;
    
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
public:
    Parser();
    /// either modify synthdefs or add a command to the chain
    void parse(const char *buf);
};




#endif /* __PARSER_H */
