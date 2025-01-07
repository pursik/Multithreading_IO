#ifndef READER_INTERFACE_H
#define READER_INTERFACE_H

#include <span>
class IReader
{
public:
    virtual ~IReader() = default;
    virtual std::span<char> Read() = 0;
};
#endif
