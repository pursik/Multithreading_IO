#pragma once

#include <span>
class IWriter
{
public:
    virtual ~IWriter() = default;
    virtual void Write(std::span<char> data) = 0;
};

