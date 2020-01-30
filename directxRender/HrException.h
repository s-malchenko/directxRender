#pragma once

#include "EasyWin.h"
#include "ExtendedException.h"

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
