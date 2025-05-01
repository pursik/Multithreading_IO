#pragma once
#include "Config.h"
#include "../FileStream/FileOperationFactory.h"
#include "../FileStream/FileDataReader.h"
#include "../FileStream/FileDataWriter.h"
#include "../FileStream/FileNameSegmentReader.h"
#include "../FileStream/FileDataSegmentReader.h"

std::shared_ptr<IReader> FileOperationFactory::CreateFileNameSegmentReader(const std::string& fileName)
{
	return FileNameSegmentReader::Create(fileName);
}

std::shared_ptr<IReader> FileOperationFactory::CreateFileDataReader(std::string_view fileName)
{
	return FileDataReader::Create(fileName, config::DefaultChunkSize);
}

std::shared_ptr<IWriter> FileOperationFactory::CreateFileDataWriter(std::string_view fileName, std::string_view subFolderName)
{
	return FileDataWriter::Create(std::string(fileName), subFolderName);
}

std::shared_ptr<IReader> FileOperationFactory::CreateFileDataSegmentReader(std::shared_ptr<IReader> reader)
{
	return FileDataSegmentReader::Create(reader);
}

