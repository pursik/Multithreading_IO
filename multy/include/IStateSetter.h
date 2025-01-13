#pragma once

class IStateSetter
{
public:
	virtual ~IStateSetter() = default;
	virtual void EnableWriting() = 0;
	virtual void EnableReading() = 0;
	virtual void DisableWriting() = 0;
	virtual void DisableReading() = 0;
};

