/**
 * @file synthdef.cpp
 * @brief  Brief description of file.
 *
 */

#include "synthdef.h"

#include "SinOsc.h"
#include "Env.h"
#include "Noise.h"

/// global directory of synths
std::map<std::string,SynthDef *>synths;

double *SinOsc::table=NULL;

/// add new synths to this function!
Gen *GenDef::build(){
    Gen *g;
    if(name == "sin")
        g = new SinOsc();
    if(name == "env")
        g = new Env();
    if(name == "noise")
        g = new Noise();
    else
        throw BadSynthException(name);
    
    
    // now set the params
    
    for(std::map<std::string, std::string>::iterator iter =
        params.begin();iter!=params.end();++iter){
        if(!g->setParam(iter->first.c_str(),iter->second.c_str())){
            throw BadParamException(name.c_str(),iter->first.c_str());
        }
    }
}


Synth *SynthDef::build(){
    // first make all the gens from the gendefs, and store
    // them in a map.
    
    std::map<std::string,Gen *> gens;
    
    for(std::map<std::string, GenDef *>::iterator iter =
        gendefs.begin();iter!=gendefs.end();++iter){
        Gen *g = iter->second->build();
        gens[iter->first]=g;
    }
    
    // now wire them together
    
    for(std::map<std::pair<std::string,std::string>,std::string>::
        iterator iter=links.begin();iter!=links.end();++iter){
        std::pair<std::string,std::string> inspec = iter->first;
        Gen *outgen = gens[iter->second];
        Gen *in = gens[inspec.first];
        in->ins[in->getInputByName(inspec.second.c_str())] = outgen->out;
    }
}
