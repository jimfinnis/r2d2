/**
 * @file exceptions.h
 * @brief  Brief description of file.
 *
 */

#ifndef __EXCEPTIONS_H
#define __EXCEPTIONS_H

#include <string>
#include <exception>

class Exception : public std::exception {
    std::string msg;
public:
    virtual ~Exception() throw() {}
    virtual const char* what() const throw(){
        return msg.c_str();
    }
};
    


class BadSynthException : public Exception {
    std::string msg;
public:
    BadSynthException(std::string name) : msg("Bad synth: "+name){}
    virtual ~BadSynthException() throw() {}
};

class BadParamException : public Exception {
    std::string msg;
public:
    BadParamException(std::string name, std::string param) 
                : msg("Bad param '"+param+"'in synth '"+name+"'"){}
    virtual ~BadParamException() throw() {}
};

class SyntaxException : public Exception {
    std::string msg;
public:
    SyntaxException() : msg("Syntax error"){}
    virtual ~SyntaxException() throw() {}
};

#endif /* __EXCEPTIONS_H */
