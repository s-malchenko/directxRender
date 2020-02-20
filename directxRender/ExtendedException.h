#pragma once

#include "EasyWin.h"
#include <exception>
#include <string>

class ExtendedException : public std::exception
{
public:
	ExtendedException(const char* file, int line, const char* desc = nullptr);
	virtual ~ExtendedException() = default;
	//get string with type of exception and origin string
	const char* what() const override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	//get string with file and line info
	std::string GetOriginString() const;
private:
	std::string description;
	std::string file;
	int line;
protected:
	mutable std::string buffer;
};

#define EXT_EXCEPT(_description) ExtendedException(__FILE__, __LINE__, (_description))
