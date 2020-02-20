#include "HrException.h"

#include <sstream>

HrException::HrException(const char* file, int line, HRESULT hr)
	: ExtendedException(file, line), hr(hr)
{
}

const char* HrException::what() const
{
	std::ostringstream ss;
	ss << GetType() << std::endl
		<< "[Error code]: " << GetErrorCode() << std::endl
		<< "[Description]: " << GetErrorString() << std::endl
		<< GetOriginString();
	buffer = ss.str();
	return buffer.c_str();
}

const char* HrException::GetType() const noexcept
{
	return "HRESULT Exception";
}

std::string HrException::TranslateErrorCode(HRESULT hr)
{
	char* buf = nullptr;
	DWORD formatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	DWORD msgLen = FormatMessage(
		formatFlags,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&buf),
		0,
		nullptr
	);

	if (msgLen == 0)
	{
		return "Unknown error code";
	}

	std::string errorString(buf);
	LocalFree(buf);
	return errorString;
}

HRESULT HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string HrException::GetErrorString() const
{
	return TranslateErrorCode(hr);
}
