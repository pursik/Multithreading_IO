#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "error_interface.h"
#include <sstream>
class ErrorCollector : public IErrorCollectorInterface
{
public:
	void AddError(std::string_view errorMessage) override;
	void PrintError() const override;
	bool Empty() const override;

private:
	std::stringstream errorCollector_;
};
#endif
