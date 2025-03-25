#pragma once

#include "IWriter.h"
#include "IReader.h"
#include <stdexcept>

class IDataAccess : public IWriter, public IReader
{
public:
	virtual ~IDataAccess() = default;
	
	virtual void Stop() = 0;

	virtual bool IsRunning() = 0;

	void Write(std::span<char> data) override 
	{ 
		throw std::logic_error("Writing not implemented.\n"); 
	}

	std::span<char> Read() override
	{
		throw std::logic_error("Reading not implemented.\n");
	}
};

