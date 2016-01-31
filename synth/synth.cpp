/**
 * @file synth.cpp
 * @brief  Brief description of file.
 *
 */

#include "synth.h"

#define MAXPROFS 1024

struct profd {
    const char *name,*genname;
    unsigned long time;
};

static int cmp_prof(const void *a,const void *b){
    const profd *x = (const profd *)a;
    const profd *y = (const profd *)b;
    return (x->time < y->time)?1:-1;
}

                  
void Synth::showprofile(){
    profd data[MAXPROFS];
    
    int ct=0;
    for(Gen *g=head;g;g=g->next){
        if(ct<MAXPROFS){
            data[ct].name = g->name;
            data[ct].genname = g->genname.c_str();
            data[ct++].time=g->proftime;
        }
    }
    
    printf("Profile------------------------------------\n");
    qsort(data,ct,sizeof(profd),cmp_prof);
    profd *p = data;
    for(int i=0;i<ct;i++,p++){
        printf("%-20s %10s %-10ld\n",p->genname,p->name,p->time);
    }
}
