#pragma once

#include "InputLayout.h"
#include <d3d11.h>
#include <wrl.h>
#include <string>

class Technique
{
public:
	struct ShaderNames
	{
		std::wstring vertexShader;
		std::wstring pixelShader;
	};

	Technique(ShaderNames shaders, const InputLayout& layout);
	void Load(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void Rebuild(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void Bind(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	ShaderNames names;
	InputLayout inputLayoutDesc;
};

