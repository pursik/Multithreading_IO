#ifndef IBUFFER_STATE_READER_H
#define IBUFFER_STATE_READER_H

class IBufferStateReader
{
public:
	virtual bool IsWritingEnabled() = 0;
	virtual bool IsReadingEnabled() = 0;
};
#endif
