#include "Camera.h"
#include "Input.h"

Camera::Camera(
	float aspectRatio,
	DirectX::XMFLOAT3 initialPosition,
	float fov,
	float nearClip,
	float farClip,
	float movementSpeed,
	float mouseLookSpeed,
	bool isPerspective)
{
	this->fov = fov;
	this->nearClip = nearClip;
	this->farClip = farClip;
	this->movementSpeed = movementSpeed;
	this->mouseLookSpeed = mouseLookSpeed;
	this->isPerspective = isPerspective;

	transform.SetPosition(initialPosition);

	UpdateProjectionMatrix(aspectRatio);
	UpdateViewMatrix();
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

Transform Camera::GetTransform()
{
	return transform;
}

float Camera::GetFOV()
{
	return fov;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	//TODO: Add orthographic support
	if (isPerspective)
		DirectX::XMStoreFloat4x4(
			&projectionMatrix,
			DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip)
		);
}

void Camera::UpdateViewMatrix()
{
	// Define Up vector
	DirectX::XMFLOAT3 worldUp(0, 1, 0);
	DirectX::XMFLOAT3 position = transform.GetPosition();
	DirectX::XMFLOAT3 forward = transform.GetForward();

	// Store matrix
	DirectX::XMStoreFloat4x4(
		&viewMatrix,
		DirectX::XMMatrixLookToLH(
			DirectX::XMLoadFloat3(&position),
			DirectX::XMLoadFloat3(&forward),
			DirectX::XMLoadFloat3(&worldUp)
		));
}

void Camera::Update(float deltaTime)
{
	bool dirtyViewMatrix = false;

	// Process user input
	Input& input = Input::GetInstance();

	// Move faster if holding shift
	if (input.KeyDown(VK_LSHIFT))
	{
		deltaTime *= 4;
	}

	// Move camera
	if (input.KeyDown('W'))
	{
		transform.MoveRelative(0, 0, movementSpeed * deltaTime);
		dirtyViewMatrix = true;
	}
	if (input.KeyDown('S'))
	{
		transform.MoveRelative(0, 0, -movementSpeed * deltaTime);
		dirtyViewMatrix = true;
	}
	if (input.KeyDown('A'))
	{
		transform.MoveRelative(-movementSpeed * deltaTime, 0, 0);
		dirtyViewMatrix = true;
	}
	if (input.KeyDown('D'))
	{
		transform.MoveRelative(movementSpeed * deltaTime, 0, 0);
		dirtyViewMatrix = true;
	}
	if (input.KeyDown(VK_SPACE))
	{
		transform.MoveRelative(0, movementSpeed * deltaTime, 0);
		dirtyViewMatrix = true;
	}
	if (input.KeyDown(VK_CONTROL))
	{
		transform.MoveRelative(0, -movementSpeed * deltaTime, 0);
		dirtyViewMatrix = true;
	}

	if (input.MouseRightDown())
	{
		// Get mouse movement
		float mouseMovementX = input.GetMouseXDelta() * mouseLookSpeed;
		float mouseMovementY = input.GetMouseYDelta() * mouseLookSpeed;

		DirectX::XMFLOAT3 currentRotation = transform.GetPitchYawRoll();
		currentRotation.x += mouseMovementY;
		currentRotation.y += mouseMovementX;

		// Clamp values to 1/2 pi and -1/2 pi
		if (currentRotation.x > DirectX::XM_PIDIV2)
			currentRotation.x = DirectX::XM_PIDIV2;
		else if (currentRotation.x < -(DirectX::XM_PIDIV2))
			currentRotation.x = -(DirectX::XM_PIDIV2);

		transform.SetRotation(currentRotation);

		dirtyViewMatrix = true;
	}

	if (dirtyViewMatrix)
		UpdateViewMatrix();
}
