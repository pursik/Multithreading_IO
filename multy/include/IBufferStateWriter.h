#ifndef IBUFFER_STATE_WRITER_H
#define IBUFFER_STATE_WRITER_H

class IBufferStateWriter
{
public:
	virtual void EnableWriting() = 0;
	virtual void EnableReading() = 0;
	virtual void DisableWriting() = 0;
	virtual void DisableReading() = 0;
};
#endif
