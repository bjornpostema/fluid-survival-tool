#ifndef LOGGER_H
#define LOGGER_H

#include <locale>

namespace model {

class Logger
{
public:
    virtual void addText(std::string str) = 0;
    virtual void addSuccess(std::string str) = 0;
    virtual void addWarning(std::string str) = 0;
    virtual void addError(std::string str) = 0;
    virtual void setText(std::string str) = 0;
    virtual std::string getText() = 0;

public:
    // virtual destructor for interface
    virtual ~Logger() { }

};

}

#endif // LOGGER_H
