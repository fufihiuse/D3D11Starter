#pragma once
#include <DirectXMath.h>

#define LIGHT_TYPE_DIR		0
#define LIGHT_TYPE_POINT	1
#define LIGHT_TYPE_SPOT		2
#define MAX_LIGHTS 128

// Make a matching struct and #defines on CPU side
struct Light
{
	int Type;						// What kind of light?
	DirectX::XMFLOAT3 Direction;	// Directional + Spot need a direction
	float Range;					// Point + Spot have a max range for attenuation
	DirectX::XMFLOAT3 Position;		// Point + Spot have a pos in space
	float Intensity;				// All lights need an intensity
	DirectX::XMFLOAT3 Color;		// All lights need a color
	float SpotFallOff;				// Spot lights need value to define cone size
	DirectX::XMFLOAT3 Padding;		// Purposefully padding to hit 16-byte boundary
	//	DirectX::XMMATRIX LightView;	// 16 bytes
	//	DirectX::XMMATRIX LightProjection;
};