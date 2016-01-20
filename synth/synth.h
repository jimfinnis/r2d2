/**
 * @file synth.h
 * @brief  A synth is made up of gens. A list of synths can be chained together, in which
 * case the next one is run when this one finishes (sets done to true).
 *
 */

#ifndef __SYNTH_H
#define __SYNTH_H

#include "gen.h"

class Synth  {
public:
    bool done; // set to true when finished
    Synth *next;
    Synth(){
        done=false;
        next=NULL;
    }
    virtual ~Synth(){}
    virtual float update()=0;
};

#endif /* __SYNTH_H */
