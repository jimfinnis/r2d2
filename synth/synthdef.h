/**
 * @file synthdef.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SYNTHDEF_H
#define __SYNTHDEF_H

#include <string>
#include <map>
#include <list>

#include "exceptions.h"
#include "synth.h"

namespace sonicAESSynth {

/// the definition of a generator, which can build generators. It's 
/// a factory with an additional map of parameters to set into
/// the constructed gen.

class GenDef {
    // map of parameters to be set into the gen once made
    std::map<std::string,std::string> params;
    std::string name; //!< name of the gen
    bool doneMon; //!< true if a "done monitor" should be generated
public:
    GenDef(std::string n)  {
        name = n;
        doneMon=false;
        out=false;
    }
    void setParam(std::string k,std::string v){
        params[k]=v;
    }
    void setDoneMon(){
        doneMon=true;
    }
    
    const char *getName(){
        return name.c_str();
    }
    
    bool out; //!< set true if this is the output from the entire synth
    Gen *build();
};

class SynthDef {
    // map of gendefs by name so we can link them together
    std::map<std::string,GenDef *> gendefs;
    GenDef *cur; //!< current gendef
    
    // list of gendefs - we need this because we need to add them
    // to the synth in order
    std::list<std::pair<std::string,GenDef *> > gendefList;
    
    /// linkage data, describing where each input comes from -
    /// it's [ (inname,inport) <- outname ] where the names
    /// are gen names.
    std::map<std::pair<std::string,std::string>,std::string> links;
    
public:
    Synth *build();
    
    /// add a new gendef, which becomes the current gendef for
    /// subsequent parameters
    GenDef *add(std::string type,std::string name){
        cur = new GenDef(type);
        gendefs[name]=cur;
        gendefList.push_front(std::make_pair(name,cur));
        return cur;
    }
    
    GenDef *findGen(std::string name){
        std::map<std::string,GenDef *>::const_iterator i;
        i=gendefs.find(name);
        if(i==gendefs.end())
            throw UnknownGenException(name);
        else
            return i->second;
    }
    
    /// create data to connect output of 
    /// "from" to input "port" on gendef "to"
    void addlink(const char *from,const char *to,const char *input){
        links[std::make_pair(to,input)]=from;
    }
    /// remove a link
    void remlink(const char *to,const char *input){
        links.erase(std::make_pair(to,input));
    }
};

/// global directory of synths
extern std::map<std::string,SynthDef *>synths;

}
#endif /* __SYNTHDEF_H */
