#pragma once

class IDataAccess 
{
public:
	virtual ~IDataAccess() = default;
	
	virtual void Stop() = 0;
	virtual bool IsRunning() = 0;
};

