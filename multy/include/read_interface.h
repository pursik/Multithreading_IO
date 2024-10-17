#ifndef READER_INTERFACE_H
#define READER_INTERFACE_H

#include <string>
class IReader
{
public:
    virtual void Read(std::string_view fileName, size_t dataSize) = 0;
};
#endif
