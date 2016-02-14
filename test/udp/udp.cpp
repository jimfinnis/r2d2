/**
 * @file mon.cpp
 * @brief  Brief description of file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <angort/angort.h>

#include "timeutils.h"
#include "udpclient.h"
#include "udpserver.h"

/// program start time
timespec progstart;

/// this is the port the monitor sends
/// messages to
#define UDPSERVER_PORT 33333

using namespace angort;

void udpwrite(const char *s,...){
    va_list args;
    va_start(args,s);
    char buf[400];
    time_t t;
    
    
    sprintf(buf,"time=%f ",gettime());
    vsnprintf(buf+strlen(buf),400-strlen(buf),s,args);
    udpSend(buf);
    va_end(args);
}

/// head of a linked list of UDP properties
static class UDPProperty *headUDPPropList=NULL;

class UDPProperty : public Property {
public:
    /// linkage field
    UDPProperty *next;
    
    /// actual value; gets copied to and from
    /// field v for Angort access
    float val;
    
    /// name of property
    const char *name;
    
    UDPProperty(const char *n){
        val=0;
        // add to list
        next=headUDPPropList;
        headUDPPropList=this;
        name = strdup(n);
    }
    
    ~UDPProperty(){
        free((void *)name);
    }
                   
    
    void set(float f){
        val = f;
    }
    
    virtual void postSet(){
        val = v.toFloat();
    }
    
    virtual void preGet(){
        Types::tFloat->set(&v,val);
    }
};

void setUDPProperty(const char *name,float val){
    for(UDPProperty *p=headUDPPropList;p;p=p->next){
        if(!strcmp(name,p->name)){
            p->val = val;
            return;
        }
    }
    throw Exception().set("cannot find UDP property %s",name);
}

void sendUDPProperties(){
    for(UDPProperty *p=headUDPPropList;p;p=p->next){
        udpwrite("%s=%f",p->name,p->val);
    }
}    


/// this handles messages coming from the monitor program,
/// and processes them to change variables or perform actions.

class MyUDPServerListener: public UDPServerListener {
    /// messages arrive as key=value pairs.
    virtual void onKeyValuePair(const char *s,float v){
        extern void setUDPProperty(const char *name,float val);
        setUDPProperty(s,v);
    }
};
MyUDPServerListener udpServerListener;


UDPServer udpServer;


%name udp
%shared

%word addvar (name --) create a new global which mirrors the monitor
{
    const StringBuffer& b = a->popString();
    a->registerProperty(b.get(),new UDPProperty(b.get()));
}

%word write (string --) write a string to the UDP port for the monitor
{
    const StringBuffer& b = a->popString();
    udpwrite(b.get());
}    

%word poll () update UDP data and send output
{
    udpServer.poll();
    sendUDPProperties();
    
    // get elapsed time
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC,&t);
    double diff=time_diff(progstart,t);
    
    udpwrite("ptime=%f",diff);
}    

%init
{
    clock_gettime(CLOCK_MONOTONIC,&progstart);
    udpServer.setListener(&udpServerListener);
    udpServer.start(UDPSERVER_PORT);
    fprintf(stderr,"Initialising UDP plugin, %s %s\n",__DATE__,__TIME__);
}
