#pragma once

#include "utility/EasyWin.h"
#include <dxgidebug.h>
#include <cstdint>
#include <string>
#include <vector>
#include <wrl.h>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	// GetMessage() will only get errors after this call
	void Set();
	std::vector<std::string> GetMessages() const;
private:
	uint64_t next = 0;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> infoQueue;
};

