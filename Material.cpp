#include "Material.h"
#include "Graphics.h"

Material::Material(
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState,
	DirectX::XMFLOAT3 colorTint,
	DirectX::XMFLOAT2 uvScale,
	DirectX::XMFLOAT2 uvOffset,
	float roughness,
	float metal
)
	:pipelineState(pipelineState), colorTint(colorTint), uvScale(uvScale), uvOffset(uvOffset), finalized(false), metal(metal), roughness(roughness)
{
	finalGPUHandleForSRVs = {};
	highestSRVSlot = 0;
	ZeroMemory(textureSRVsBySlot, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE) * 4);
}

D3D12_GPU_DESCRIPTOR_HANDLE Material::GetFinalGPUHandleForSRVs()
{
	return finalGPUHandleForSRVs;
}

void Material::AddTexture(D3D12_CPU_DESCRIPTOR_HANDLE srv, int slot)
{
	// Valid slot?
	if (finalized || slot < 0 || slot >= 128)
		return;

	// Save and check if this was the highest slot
	textureSRVsBySlot[slot] = srv;
	highestSRVSlot = max(highestSRVSlot, slot);
}

void Material::FinalizeMaterial()
{
	if (finalized) return;

	for (int i = 0; i < highestSRVSlot; i++)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = Graphics::CopySRVsToDescriptorHeapAndGetGPUDescriptorHandle(textureSRVsBySlot[i], 1);
		if (i == 0)
			finalGPUHandleForSRVs = gpuHandle;
	}

	finalized = true;
}

DirectX::XMFLOAT3 Material::GetColorTint()
{
	return colorTint;
}

void Material::SetColorTint(DirectX::XMFLOAT3 colorTint)
{
	this->colorTint = colorTint;
}

float Material::GetRoughness()
{
	return roughness;
}

float Material::GetMetal()
{
	return metal;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
	return uvScale;
}

void Material::SetUVScale(DirectX::XMFLOAT2 uvScale)
{
	this->uvScale = uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
	return uvOffset;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 uvOffset)
{
	this->uvOffset = uvOffset;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> Material::GetPipelineState()
{
	return pipelineState;
}

void Material::SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState)
{
	this->pipelineState = pipelineState;
}
