#include "ExtendedException.h"

#include <sstream>

ExtendedException::ExtendedException(const char * file, int line)
	: file(file), line(line)
{
}

const char* ExtendedException::what() const
{
	std::ostringstream ss;
	ss << GetType() << std::endl << GetOriginString();
	buffer = ss.str();
	return buffer.c_str();
}

const char * ExtendedException::GetType() const noexcept
{
	return "Extended Exception";
}

int ExtendedException::GetLine() const noexcept
{
	return line;
}

const std::string & ExtendedException::GetFile() const noexcept
{
	return file;
}

std::string ExtendedException::GetOriginString() const
{
	std::ostringstream ss;
	ss << "[File]: " << GetFile() << std::endl
		<< "[Line]: " << GetLine();
	return ss.str();
}
