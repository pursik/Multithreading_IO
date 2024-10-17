#ifndef ERROR_H
#define ERROR_H

#include <string>
class IErrorCollectorInterface
{
public:
	virtual void AddError(std::string_view errorMessage) = 0;
	virtual void PrintError() const = 0;
	virtual bool Empty() const = 0;
};
#endif
