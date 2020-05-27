#include "InputLayout.h"
#include <cassert>

InputLayout::InputLayout(const D3D11_INPUT_ELEMENT_DESC* descs, size_t num)
{
	if (descs == nullptr)
	{
		return;
	}

	for (size_t i = 0; i < num; ++i)
	{
		elements.push_back(descs[i]);
		names.emplace_back(elements.back().SemanticName);
	}
}

const D3D11_INPUT_ELEMENT_DESC* InputLayout::GetLayout() const
{
	for (size_t i = 0; i < elements.size(); ++i)
	{
		elements[i].SemanticName = names[i].data();
	}

	return elements.data();
}

uint32_t InputLayout::GetSize() const
{
	return static_cast<uint32_t>(elements.size());
}
