#pragma once
#include "Config.h"
#include "../FileStream/FileOperationFactory.h"
#include "../FileStream/FileDataReader.h"
#include "../FileStream/FileDataWriter.h"
#include "../FileStream/FileNameSegmentReader.h"
#include "../FileStream/FileDataSegmentReader.h"

std::shared_ptr<IReader> FileOperationFactory::CreateFileNameSegmentReader(std::string_view fileName, std::shared_ptr<IEncrypt> encryptor)
{
	return FileNameSegmentReader::Create(fileName, encryptor);
}

std::shared_ptr<IReader> FileOperationFactory::CreateFileDataReader(std::string_view fileName)
{
	return FileDataReader::Create(fileName, config::DefaultChunkSize);
}

std::shared_ptr<IWriter> FileOperationFactory::CreateFileDataWriter(std::string_view fileName, std::string_view subFolderName)
{
	return FileDataWriter::Create(std::string(fileName), subFolderName);
}

std::shared_ptr<IReader> FileOperationFactory::CreateFileDataSegmentReader(std::shared_ptr<IReader> reader, std::shared_ptr<IEncrypt> encryptor)
{
	return FileDataSegmentReader::Create(reader, encryptor);
}

