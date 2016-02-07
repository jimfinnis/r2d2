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

namespace sonicAESSynth {

/// a synthesiser (a collection of Gen) instantiated by a
/// SynthDef (a collection of GenDef)

class Synth  {
    Gen *head,*tail;
    Gen *out;
public:
    bool done; // set to true when finished
    Synth(){
        head=NULL;
        done=false;
        out=NULL;
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
            throw Exception("Update too big for maxframesize");
        }
        
        done=true;
        for(Gen *g=head;g;g=g->next){
            g->startprof();
            g->update(nframes);
            g->endprof();
            if(g->isDoneMon && !g->done)
                done=false;
        }
    }
    
    /// get the output gen's buffer
    virtual float *getout(){
        if(!out)
            throw Exception("no output in synth");
        return out->out;
    }
    
#ifdef PROFILING
    void showprofile();
    
#endif
};
}
#endif /* __SYNTH_H */
