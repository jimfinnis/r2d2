/**
 * @file synth.h
 * @brief  A synth makes a sound using gens and is instantiated by
 * a synthdef.
 *
 */

#ifndef __SYNTH_H
#define __SYNTH_H

#include <stdio.h>
#include "gen.h"

class Synth  {
    Gen *head,*tail;
    Gen *out;
public:
    bool done; // set to true when finished
    Synth(){
        head=NULL;
        done=false;
    }
    
    // add generators in reverse order of execution, ideally
    
    void add(Gen *g,bool isout=false){
        if(!head)tail=g;
        g->next = head;
        head = g;
        if(isout)out=g;
    }
    
    virtual ~Synth(){
        Gen *p=head;
        while(p){
            Gen *q = p->next;
            delete p;
            p=q;
        }
    }

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
    
    /// get the output gen's buffer
    virtual float *getout(){
        return out->out;
    }
};

#endif /* __SYNTH_H */
