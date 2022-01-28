#pragma once

#include "renderSystem/EasyD3D11.h"
#include <wrl.h>

class Material
{
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureView;
};