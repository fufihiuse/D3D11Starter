#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

class Material
{
private:
	DirectX::XMFLOAT3 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	float roughness;
	float metal;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVsBySlot[128];
	D3D12_GPU_DESCRIPTOR_HANDLE finalGPUHandleForSRVs;
	bool finalized;
	int highestSRVSlot;

public:
	Material(
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState, 
		DirectX::XMFLOAT3 colorTint = DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT2 uvScale = DirectX::XMFLOAT2( 1, 1 ),
		DirectX::XMFLOAT2 uvOffset = DirectX::XMFLOAT2( 0, 0 ),
		float roughness = 1.0f,
		float metal = 0.0f
	);
	D3D12_GPU_DESCRIPTOR_HANDLE GetFinalGPUHandleForSRVs();
	void AddTexture(D3D12_CPU_DESCRIPTOR_HANDLE srv, int slot);
	void FinalizeMaterial();

	DirectX::XMFLOAT3 GetColorTint();
	void SetColorTint(DirectX::XMFLOAT3 colorTint);

	float GetRoughness();
	float GetMetal();

	DirectX::XMFLOAT2 GetUVScale();
	void SetUVScale(DirectX::XMFLOAT2 uvScale);

	DirectX::XMFLOAT2 GetUVOffset();
	void SetUVOffset(DirectX::XMFLOAT2 uvOffset);

	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState();
	void SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState);

};

