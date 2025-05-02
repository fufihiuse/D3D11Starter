#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

class Material
{
private:
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	bool finalized;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVsBySlot[4];
	D3D12_GPU_DESCRIPTOR_HANDLE finalGPUHandleForSRVs;

public:
	Material(
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState, 
		DirectX::XMFLOAT4 colorTint = DirectX::XMFLOAT4(0, 0, 0, 1),
		DirectX::XMFLOAT2 uvScale = DirectX::XMFLOAT2( 1, 1 ),
		DirectX::XMFLOAT2 uvOffset = DirectX::XMFLOAT2( 0, 0 )
	);
	D3D12_GPU_DESCRIPTOR_HANDLE GetFinalGPUHandleForSRVs();
	void AddTexture(D3D12_CPU_DESCRIPTOR_HANDLE srv, int slot);
	void FinalizeMaterial();

	DirectX::XMFLOAT4 GetColorTint();
	void SetColorTint(DirectX::XMFLOAT4 colorTint);

	DirectX::XMFLOAT2 GetUVScale();
	void SetUVScale(DirectX::XMFLOAT2 uvScale);

	DirectX::XMFLOAT2 GetUVOffset();
	void SetUVOffset(DirectX::XMFLOAT2 uvOffset);

	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState();
	void SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState);

};

