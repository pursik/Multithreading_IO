#pragma once
#include "IDataAccess.h"
#include "IReader.h"
class IReadDataAccess : public IDataAccess, public IReader
{
};

