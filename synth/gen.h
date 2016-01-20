/**
 * @file gen.h
 * @brief  Interface for elements of a synth
 *
 */

#ifndef __GEN_H
#define __GEN_H

#include <math.h>

class Gen {
public:
    virtual ~Gen(){}
    virtual float update() = 0;
    Gen *next;
};




#endif /* __GEN_H */
