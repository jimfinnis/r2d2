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


Synth *SynthDef::build(){
    Synth *s = new Synth();
    
    // first make all the gens from the gendefs, and store
    // them in a map.
    
    std::map<std::string,Gen *> gens;
    
    for(std::map<std::string, GenDef *>::iterator iter =
        gendefs.begin();iter!=gendefs.end();++iter){
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
        Gen *outgen = gens[iter->second];
        Gen *in = gens[inspec.first];
        in->ins[in->getInputByName(inspec.second.c_str())] = outgen->out;
    }
    return s;
}
