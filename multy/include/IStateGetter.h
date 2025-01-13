#pragma once

class IStateGetter
{
public:
	virtual ~IStateGetter() = default;
	virtual bool IsWritingEnabled() = 0;
	virtual bool IsReadingEnabled() = 0;
};

