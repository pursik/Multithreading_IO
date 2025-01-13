#pragma once
class IBuffer
{
public:
	virtual ~IBuffer() = default;
	virtual char& operator[](size_t index) = 0;
	virtual void Write(char item) = 0;
	virtual size_t GetDataSize() const = 0;
	virtual size_t GetHead() const = 0;
	virtual void SetHead(size_t value) = 0;

protected:
	size_t head_ = 0;
	size_t tail_ = 0;
};
