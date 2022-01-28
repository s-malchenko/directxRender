#include "TechniqueFactory.h"

namespace TechniqueFactory
{
	static const std::vector<std::string> k_availableTechniques =
	{
		"color",
		"normal",
	};

	static constexpr int k_defTechIndex = 1;

	static const D3D11_INPUT_ELEMENT_DESC k_defLayoutDesc[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static const InputLayout k_defLayout = InputLayout(k_defLayoutDesc, std::size(k_defLayoutDesc));

	const std::vector<std::string>& GetAvailableTechniques()
	{
		return k_availableTechniques;
	}

	Technique CreateTechnique(const std::string& name)
	{
		return Technique(name, k_defLayout);
	}

	Technique CreateDefaultTechnique()
	{
		return CreateTechnique(k_availableTechniques[k_defTechIndex]);
	}
}