#ifndef WRITER_INTERFACE_H
#define WRITER_INTERFACE_H

#include <string>
class IWriter
{
public:
    virtual void Write(std::string_view fileName) = 0;
};
#endif
