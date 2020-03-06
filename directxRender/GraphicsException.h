#pragma once

#include "DxgiInfoManager.h"
#include "HrException.h"

class GraphicsException : public HrException
{
public:
	GraphicsException(const char* file, int line, HRESULT hr, const std::vector<std::string>& messages = {});
	const char* what() const override;
	const char* GetType() const noexcept override;
	const std::string& GetErrorInfo() const;
#ifndef NDEBUG
	static DxgiInfoManager infoManager;
#endif
private:
	std::string info;
};

class DeviceRemovedException : public GraphicsException
{
public:
	DeviceRemovedException(const char* file, int line, HRESULT hr, const std::vector<std::string>& messages = {});
	const char* GetType() const noexcept override;
};

#include "GfxMacros.h"
