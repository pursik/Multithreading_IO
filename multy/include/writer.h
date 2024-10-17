#ifndef WRITER_H
#define WRITER_H

#include "write_interface.h"
#include "buffer_interface.h"
#include "error_interface.h"

class Writer : public IWriter
{
private:
    IBuffer &sharedBuffer_;
    IErrorCollectorInterface &errorCollector_;

public:
    Writer(IBuffer &buf, IErrorCollectorInterface &errorCollector);

    void Write(std::string_view fileName) override;
};
#endif
