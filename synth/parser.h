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
    NoteCmd(Synth *s,double f){
        synth = s;
        freq = f;
        next = NULL;
    }
              
    
    Synth *synth; //!< is deleted with this structure
    double freq;
    NoteCmd *next;
};

class Parser {
private:
    Tokeniser tok;
    std::string getnextident(){
        if(tok.getnext()!=T_IDENT)
            throw SyntaxException();
        return std::string(tok.getstring());
    }
public:
    Parser();
    /// either modify synthdefs or add a command to the chain
    void parse(const char *buf);
};

    


#endif /* __PARSER_H */
