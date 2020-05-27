#pragma once

#include "utility/EasyWin.h"

#include "exception/ExtendedException.h"

class HrException : public ExtendedException
{
public:
	HrException(const char* file, int line, HRESULT hr);
	const char* what() const override;
	const char* GetType() const noexcept override;
	static std::string TranslateErrorCode(HRESULT hr);
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorString() const;
private:
	HRESULT hr;
};

#define HR_EXCEPT_LASTERR() HrException(__FILE__, __LINE__, GetLastError())
