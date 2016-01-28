/**
 * @file synth.h
 * @brief  A synth makes a sound using gens.  A list of synths can be chained together, in which
 * case the next one is run when this one finishes (sets done to true).
 * 
 * The synth itself can be thought of as a patch, but the parameters
 * of the gens are separate (since they are shared between all synths).
 *
 */

#ifndef __SYNTH_H
#define __SYNTH_H

#include "gen.h"

class Synth  {
    Gen *head,*tail;
public:
    bool done; // set to true when finished
    Synth *next;
    Synth(){
        next=NULL;
        head=NULL;
        done=false;
    }
    
    // add generators in reverse order of execution (i.e. output
    // generator first). If donemon is set, this is considered an
    // "isdone monitor". All these must set their done flags to set
    // the synth's done flag. Typically used of Envs.
    
    void add(Gen *g,bool donemon=false){
        if(!head)tail=g;
        g->next = head;
        head = g;
        g->isDoneMon = donemon;
    }
    
    virtual ~Synth(){}

    virtual void update(int nframes) {
        if(nframes>MAXFRAMESIZE){
            printf("%d is too big (maxframesize %d)\n",
                   nframes,MAXFRAMESIZE);
            exit(1);
        }
        
        done=true;
        for(Gen *g=head;g;g=g->next){
            g->update(nframes);
            if(g->isDoneMon && !g->done)
                done=false;
        }
    }
    
    /// the output is always the tail gen (the first added)
    virtual double *getout(){
        return tail->out;
    }
};

#endif /* __SYNTH_H */
