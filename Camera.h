#pragma once
#include <DirectXMath.h>
#include "Transform.h"

class Camera
{
private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	float fov;
	float nearClip;
	float farClip;
	float movementSpeed;
	float mouseLookSpeed;
	bool isPerspective; // If true, then perspective; If false, then ortho

public:
	Camera(
		float aspectRatio,
		DirectX::XMFLOAT3 initialPosition = DirectX::XMFLOAT3(0, 0, -1),
		float fov = 45,
		float nearClip = 0.01f,
		float farClip = 1000.0f,
		float movementSpeed = 0.5f,
		float mouseLookSpeed = 0.02f,
		bool isPerspective = true
	);

	// Getters
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	Transform GetTransform();
	float GetFOV();

	// Matrix Updates
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();

	// Update
	void Update(float deltaTime);
};

