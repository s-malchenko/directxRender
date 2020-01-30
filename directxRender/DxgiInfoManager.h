#pragma once

#include "EasyWin.h"
#include <vector>
#include <string>
#include <cstdint>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	// GetMessage() will only get errors after this call
	void Set();
	std::vector<std::string> GetMessages() const;
private:
	uint64_t next = 0;
	struct IDXGIInfoQueue* pDxgiInfoQueue = nullptr;
};

