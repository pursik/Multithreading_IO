#pragma once
#include "IDataAccess.h"
#include "IWriter.h"

class IWriteDataAccess : public IDataAccess, public IWriter
{
};

