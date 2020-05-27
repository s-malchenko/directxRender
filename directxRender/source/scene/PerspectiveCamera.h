#pragma once

#include <DirectXMath.h>

class PerspectiveCamera
{
public:
	PerspectiveCamera(float fov, float aspect, float nearPlane = 0.1, float farPlane = 500);
	void SetFov(float newFov) noexcept;
	void SetAspect(float newAspect) noexcept;
	void SetSpeed(float newSpeed) noexcept;
	void MultiplySpeed(float speedMul) noexcept;
	void Move(float dx, float dy, float dz) noexcept;
	void Turn(float dPitch, float dRoll) noexcept;
	float GetSpeed() const noexcept;
	DirectX::XMMATRIX GetPerspectiveViewTransform() const;
private:
	DirectX::XMMATRIX worldTransform;
	float fov, aspect, nearPlane, farPlane;
	float pitch = 0;
	float speed = 1;
};

