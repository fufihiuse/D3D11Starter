#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	translation(0, 0, 0),
	pitchYawRoll(0, 0, 0),
	scale(1, 1, 1),
	matrixDirty(false)
{
	// Create an identity matrix as an XMMATRIX (math type)
	// and immediately store in the storage type
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix,
		XMMatrixInverse(0, XMMatrixTranspose(XMMatrixIdentity())));
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return translation;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	// Because updating every time you change would be expensive, possibly multiple times per frame;
	// only update when you need it
	CalculateWorldMatrix();
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return worldInverseTransposeMatrix;
}

bool Transform::IsMatrixDirty()
{
	return matrixDirty;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	// Start with default forward vector
	XMVECTOR rgt = XMVectorSet(1, 0, 0, 0);

	// Build a quaternion that matches our pitch/yaw/roll rotation
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// Rotate our "default forward" by our actual rotation
	XMFLOAT3 rightVector;
	XMStoreFloat3(&rightVector, XMVector3Rotate(rgt, rotQuat));
	return rightVector;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	// Start with default forward vector
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	// Build a quaternion that matches our pitch/yaw/roll rotation
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// Rotate our "default forward" by our actual rotation
	XMFLOAT3 upVector;
	XMStoreFloat3(&upVector, XMVector3Rotate(up, rotQuat));
	return upVector;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	// Start with default forward vector
	XMVECTOR fwd = XMVectorSet(0, 0, 1, 0);

	// Build a quaternion that matches our pitch/yaw/roll rotation
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// Rotate our "default forward" by our actual rotation
	XMFLOAT3 forwardVector;
	XMStoreFloat3(&forwardVector, XMVector3Rotate(fwd, rotQuat));
	return forwardVector;
}

void Transform::SetPosition(float x, float y, float z)
{
	translation.x = x;
	translation.y = y;
	translation.z = z;
	matrixDirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	translation.x = position.x;
	translation.y = position.y;
	translation.z = position.z;
	matrixDirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll.x = pitch;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
	matrixDirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll.x = rotation.x;
	pitchYawRoll.y = rotation.y;
	pitchYawRoll.z = rotation.z;
	matrixDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	matrixDirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale.x = scale.x;
	this->scale.y = scale.y;
	this->scale.z = scale.z;
	matrixDirty = true;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	translation.x += x;
	translation.y += y;
	translation.z += z;
	matrixDirty = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
	// Create rotation quaternion
	XMVECTOR movement = XMVectorSet(x, y, z, 0);
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// Rotate movement to make it relative
	XMVECTOR dir = XMVector3Rotate(movement, rotQuat);

	// Add rotated vector to pos
	XMStoreFloat3(&translation, XMLoadFloat3(&translation) + dir);

	matrixDirty = true;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x += pitch;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;
	matrixDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	matrixDirty = true;
}

void Transform::CalculateWorldMatrix()
{
	if (!matrixDirty)
		return;
	// Create the three matrices that make up world matrix
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&translation));
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

	// Combine into a single world matrix
	XMMATRIX worldMat = scaleMatrix * rotationMatrix * translationMatrix;
	XMStoreFloat4x4(&worldMatrix, worldMat);
	XMStoreFloat4x4(&worldInverseTransposeMatrix,
		XMMatrixInverse(0, XMMatrixTranspose(worldMat)));
	matrixDirty = false;
}
