#include "PerspectiveCamera.h"

#include <algorithm>
#include <cmath>

using namespace DirectX;

PerspectiveCamera::PerspectiveCamera(float fov)
	: fov(fov)
{
	worldTransform = XMMatrixTranslation(0, 0, 4);
}

void PerspectiveCamera::Reset()
{
	*this = PerspectiveCamera();
}

void PerspectiveCamera::SetFov(float newFov) noexcept
{
	fov = newFov;
}

void PerspectiveCamera::SetSpeed(float newSpeed) noexcept
{
	speed = newSpeed;
}

void PerspectiveCamera::MultiplySpeed(float speedMul) noexcept
{
	speed *= speedMul;
}

void PerspectiveCamera::Move(float forward, float side, float up) noexcept
{
	forward *= speed;
	side *= speed;
	up *= speed;
	worldTransform *= XMMatrixTranslation(-side, 0, -forward);

	XMVECTOR worldZ = { 0, std::cos(pitch), std::sin(pitch) };
	worldTransform *= XMMatrixTranslationFromVector(worldZ * -up);
}

void PerspectiveCamera::Turn(float dPitch, float dYaw) noexcept
{
	auto prevPitch = pitch;
	pitch = std::clamp(pitch + dPitch, -XM_PIDIV2, XM_PIDIV2);
	dPitch = pitch - prevPitch;
	worldTransform *= XMMatrixRotationX(dPitch);

	XMVECTOR worldZ = { 0, std::cos(pitch), std::sin(pitch) };
	worldTransform *= XMMatrixRotationAxis(worldZ, -dYaw);
}

float PerspectiveCamera::GetSpeed() const noexcept
{
	return speed;
}

XMMATRIX PerspectiveCamera::GetPerspectiveViewTransform(float aspect, float nearPlane, float farPlane) const
{
	return worldTransform * XMMatrixPerspectiveFovLH(fov, aspect, nearPlane, farPlane);
}
