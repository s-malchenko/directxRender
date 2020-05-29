#pragma once

#include <DirectXMath.h>

template<class T>
struct SceneObject
{
	T object;
	DirectX::XMMATRIX transform;
};
