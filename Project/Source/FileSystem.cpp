#include "FileSystem.h"

#include<filesystem>

bool IsRegularFile(const char* FileName) {
	return std::filesystem::is_regular_file(FileName);
}

bool InputFileSystem::Open(const char* FileName) {
	if (!IsRegularFile(FileName)) {
		return false;
	}
	fileBuffer.open(FileName, std::ios_base::in);
	return true;
}

void InputFileSystem::Close() {
	fileBuffer.close();
}

bool InputFileSystem::IsEof() {
	return fileBuffer.eof();
}

void InputFileSystem::GetLine(char** LineStr) {
	fileBuffer.getline(*LineStr, sizeof(*LineStr));
}

bool OutputFileSystem::Open(const char* FileName) {
	fileBuffer.open(FileName, std::ios_base::out);
	return true;
}

void OutputFileSystem::Close() {
	fileBuffer.close();
}

void OutputFileSystem::AddLine(const char* LineStr) {
	fileBuffer << LineStr << std::endl;
}
