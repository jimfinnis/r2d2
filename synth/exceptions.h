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
    Exception(std::string m){
        msg = m;
    }
    virtual ~Exception() throw() {}
    virtual const char* what() const throw(){
        return msg.c_str();
    }
};

class BadSynthException : public Exception {
public:
    BadSynthException(std::string name) : Exception("Bad synth: "+name){}
};

class BadParamException : public Exception {
public:
    BadParamException(std::string name, std::string param)
                : Exception("Bad param '"+param+"' in gen '"+name+"'"){}
};

class SyntaxException : public Exception {
public:
    SyntaxException() : Exception("Syntax error"){}
};

class UnexpectedException : public Exception {
public:
    UnexpectedException(std::string expected, const char *got) : 
    Exception("Syntax error: expected "+expected+", got "+std::string(got)){}
};

class UnknownInputException : public Exception {
public:
    UnknownInputException(const char *name,const char *input) 
                : Exception("Unknown input "+std::string(input)+
                            " in gen "+std::string(name)) {}
};

class BadTimeException : public Exception {
public:
    BadTimeException(std::string name)
                : Exception("bad time in envelope "+name){}
};

class UnknownGenException : public Exception {
public:
    UnknownGenException(std::string name)
                : Exception("unknown gen "+name){}
};

class NoteException : public Exception {
public:
    NoteException(std::string name)
                : Exception("unknown note word '"+name+"'"){}
};

class StackUnderflowException : public Exception {
public:
    StackUnderflowException() : Exception("stack underflow"){}
};

class StackOverflowException : public Exception {
public:
    StackOverflowException() : Exception("stack Overflow"){}
};
    

#endif /* __EXCEPTIONS_H */
