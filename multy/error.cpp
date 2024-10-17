#include "error.h"
#include <iostream>
void ErrorCollector::AddError(std::string_view errorMessage)
{
	errorCollector_ << std::endl
					<< std::string(errorMessage);
}
void ErrorCollector::PrintError() const
{
	std::cerr << "Errors: " << errorCollector_.str() << std::endl;
}
bool ErrorCollector::Empty() const
{
	return errorCollector_.str().empty();
}