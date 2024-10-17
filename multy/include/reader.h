#ifndef READER_H
#define READER_H

#include "read_interface.h"
#include "buffer_interface.h"
#include "error_interface.h"

class Reader : public IReader
{
private:
	IBuffer &sharedBuffer_;
	IErrorCollectorInterface &errorCollector_;

public:
	Reader(IBuffer &buf, IErrorCollectorInterface &errHandler);

	void Read(std::string_view fileName, size_t dataSize) override;
};
#endif
