/**
 * @file parser.h
 * @brief  Builds synths with float parameters from a string
 *
 */

#ifndef __PARSER_H
#define __PARSER_H

#include "synth.h"
#include "tokeniser.h"

class Parser {
private:
    Tokeniser tok;
    Synth *parseSynth();
public:
    Parser();
    Synth *parse(const char *buf);
};

    


#endif /* __PARSER_H */
