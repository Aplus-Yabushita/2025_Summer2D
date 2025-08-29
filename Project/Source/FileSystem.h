#pragma once

#include <fstream>

extern bool IsRegularFile(const char* FileName);

class FileSystem {
protected:
	std::fstream fileBuffer;
public:
	FileSystem(void) noexcept {}
	FileSystem(const FileSystem&) = delete;
	FileSystem(FileSystem&&) = delete;
	FileSystem& operator=(const FileSystem&) = delete;
	FileSystem& operator=(FileSystem&&) = delete;
	virtual ~FileSystem(void) noexcept {}
public:
	virtual bool Open(const char* FileName) = 0;
	virtual void Close() = 0;
};
class InputFileSystem :public FileSystem {
public:
	InputFileSystem(void) noexcept {}
	InputFileSystem(const InputFileSystem&) = delete;
	InputFileSystem(InputFileSystem&&) = delete;
	InputFileSystem& operator=(const InputFileSystem&) = delete;
	InputFileSystem& operator=(InputFileSystem&&) = delete;
	virtual ~InputFileSystem(void) noexcept {}
public:
	virtual bool Open(const char* FileName);
	virtual void Close();
public:
	bool IsEof();
	void GetLine(char** LineStr);
};

class OutputFileSystem :public FileSystem {
public:
	OutputFileSystem(void) noexcept {}
	OutputFileSystem(const OutputFileSystem&) = delete;
	OutputFileSystem(OutputFileSystem&&) = delete;
	OutputFileSystem& operator=(const OutputFileSystem&) = delete;
	OutputFileSystem& operator=(OutputFileSystem&&) = delete;
	virtual ~OutputFileSystem(void) noexcept {}
public:
	virtual bool Open(const char* FileName);
	virtual void Close();
public:
	void AddLine(const char* LineStr);
};
