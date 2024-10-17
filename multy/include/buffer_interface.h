#ifndef BUFFER_INTERFACE_H
#define BUFFER_INTERFACE_H

#include <vector>
class IBuffer
{
public:
    virtual ~IBuffer() {}

    virtual void Refill(const std::vector<char> &data) = 0;
    virtual std::vector<char> Retrieve() = 0;
    virtual bool Empty() = 0;
    virtual bool IsBufferRefillingStopped() = 0;
    virtual void StopBufferRefilling() = 0;
};
#endif
