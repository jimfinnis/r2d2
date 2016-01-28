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

class Parser {
private:
    Tokeniser tok;
    std::string getnextident(){
        if(tok.getnext()!=T_IDENT)
            throw SyntaxException();
        return std::string(tok.getstring());
    }
    Synth *parseSynth();
public:
    Parser();
    Synth *parse(const char *buf);
};

    


#endif /* __PARSER_H */
