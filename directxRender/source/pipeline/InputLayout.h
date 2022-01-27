#pragma once

#include "renderSystem/EasyD3D11.h"
#include <string>
#include <vector>

class InputLayout
{
public:
	InputLayout(const D3D11_INPUT_ELEMENT_DESC* descs, size_t num);
	~InputLayout() = default;
	const D3D11_INPUT_ELEMENT_DESC* GetLayout() const;
	uint32_t GetSize() const;
private:
	std::vector<std::string> names;
	mutable std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
};
