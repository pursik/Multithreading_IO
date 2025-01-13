#pragma once

class IProcessing
{
public:
	virtual ~IProcessing() = default;
	virtual void Reading() = 0;
	virtual void Writing() = 0;
};

