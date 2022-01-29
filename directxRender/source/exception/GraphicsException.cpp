#include "GraphicsException.h"

#include <sstream>

GraphicsException::GraphicsException(const char* file, int line, HRESULT hr, const std::vector<std::string>& messages)
	: HrException(file, line, hr)
{
	std::ostringstream ss;

	for (const auto& msg : messages)
	{
		ss << msg << std::endl;
	}

	info = ss.str();
}

const char* GraphicsException::what() const
{
	if (!info.empty())
	{
		std::ostringstream ss;
		ss << HrException::what() << std::endl << info;
		buffer = ss.str();
		return buffer.c_str();
	}

	return HrException::what();
}

const char* GraphicsException::GetType() const noexcept
{
	return "Graphics Exception";
}

const std::string& GraphicsException::GetErrorInfo() const
{
	return info;
}

DeviceRemovedException::DeviceRemovedException(const char* file, int line, HRESULT hr, const std::vector<std::string>& messages)
	: GraphicsException(file, line, hr, messages)
{
}

const char* DeviceRemovedException::GetType() const noexcept
{
	return "DeviceRemoved Exception";
}

#ifndef NDEBUG
DxgiInfoManager GraphicsException::infoManager;
#endif
