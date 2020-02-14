#include "PerspectiveCamera.h"

#include <algorithm>

using namespace DirectX;

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane)
	: fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane)
{
	worldTransform = XMMatrixTranslation(0, -4, 0) * XMMatrixRotationX(-XM_PIDIV2);
}

void PerspectiveCamera::SetFov(float newFov) noexcept
{
	fov = newFov;
}

void PerspectiveCamera::SetAspect(float newAspect) noexcept
{
	aspect = newAspect;
}

void PerspectiveCamera::Move(float forward, float side, float up) noexcept
{
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

XMMATRIX PerspectiveCamera::GetPerspectiveViewTransform() const
{
	return worldTransform * XMMatrixPerspectiveFovLH(fov, aspect, nearPlane, farPlane);
}
