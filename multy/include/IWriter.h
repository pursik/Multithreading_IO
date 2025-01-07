#ifndef WRITER_INTERFACE_H
#define WRITER_INTERFACE_H

#include <span>
class IWriter
{
public:
    virtual ~IWriter() = default;
    virtual void Write(std::span<char> data) = 0;
};
#endif
