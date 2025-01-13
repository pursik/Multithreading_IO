#pragma once
#include <span>
class IReader
{
public:
    virtual ~IReader() = default;
    virtual std::span<char> Read() = 0;
};

