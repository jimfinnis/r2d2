/**
 * @file synthdef.cpp
 * @brief  Brief description of file.
 *
 */

#include "synthdef.h"

#include "SinOsc.h"
#include "Env.h"
#include "Noise.h"
#include "Utils.h"
#include "WaveTableOsc.h"
#include "Perlin.h"
#include "LPF.h"

/// global directory of synths
std::map<std::string,SynthDef *>synths;

float *SinOsc::table=NULL;

/// add new synths to this function!
Gen *GenDef::build(){
    Gen *g;
    if(name == "sin")
        g = new SinOsc();
    else if(name == "env")
        g = new Env();
    else if(name == "noise")
        g = new Noise();
    else if(name == "mix2")
        g = new ConstMix();
    else if(name == "wave")
        g = new WaveTableOsc();
    else if(name == "perlin")
        g = new Perlin();
    else if(name == "lpf")
        g = new LPF();
    else
        throw BadSynthException(name);
    
    if(doneMon)g->isDoneMon=true;
    
    // now set the params
    
    for(std::map<std::string, std::string>::iterator iter =
        params.begin();iter!=params.end();++iter){
        if(!g->setParam(iter->first.c_str(),iter->second.c_str())){
            throw BadParamException(name.c_str(),iter->first.c_str());
        }
    }
    return g;
}

inline Gen *findGenInMap(std::map<std::string,Gen *> &map,std::string name){
    std::map<std::string,Gen *>::const_iterator i;
    i = map.find(name);
    if(i==map.end())
        throw UnknownGenException(name);
    else
        return i->second;
}

Synth *SynthDef::build(){
    Synth *s = new Synth();
    
    // first make all the gens from the gendefs, and store
    // them in a map.
    
    std::map<std::string,Gen *> gens;
    
    // iterate through the list (which maintains the insert order)
    
    for(std::list<std::pair<std::string, GenDef *> >::iterator iter =
        gendefList.begin();iter!=gendefList.end();++iter){
        GenDef *d = iter->second;
        Gen *g = d->build();
        gens[iter->first]=g;
        g->setName(iter->first);
        // add to the synth, setting the output
        s->add(g,d->out);
    }
    
    // now wire them together
    
    for(std::map<std::pair<std::string,std::string>,std::string>::
        iterator iter=links.begin();iter!=links.end();++iter){
        std::pair<std::string,std::string> inspec = iter->first;
        Gen *outgen = findGenInMap(gens,iter->second);
        Gen *in = findGenInMap(gens,inspec.first);
        in->ins[in->getInputByName(inspec.second.c_str())] = outgen->out;
    }
    return s;
}
