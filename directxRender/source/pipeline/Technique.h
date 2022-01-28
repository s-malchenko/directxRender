#pragma once

#include "pipeline/InputLayout.h"
#include "renderSystem/EasyD3D11.h"
#include <wrl.h>
#include <string>

class Technique
{
public:

	Technique(const std::string& name, const InputLayout& layout);
	void Load(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void Rebuild(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void Bind(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const;
	const std::string& GetName() const { return mName; }
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	InputLayout inputLayoutDesc;
	const std::string mName;
};

