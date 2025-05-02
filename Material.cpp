#include "Material.h"
#include "Graphics.h"

Material::Material(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState, DirectX::XMFLOAT4 colorTint, DirectX::XMFLOAT2 uvScale, DirectX::XMFLOAT2 uvOffset) 
	:pipelineState(pipelineState), colorTint(colorTint), uvScale(uvScale), uvOffset(uvOffset), finalized(false)
{
	finalGPUHandleForSRVs = {};
	ZeroMemory(textureSRVsBySlot, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE) * 4);
}

D3D12_GPU_DESCRIPTOR_HANDLE Material::GetFinalGPUHandleForSRVs()
{
	return finalGPUHandleForSRVs;
}

void Material::AddTexture(D3D12_CPU_DESCRIPTOR_HANDLE srv, int slot)
{
	// Prevent adding after material finished or 
	textureSRVsBySlot[slot] = srv;
}

void Material::FinalizeMaterial()
{
	if (finalized) return;

	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
			finalGPUHandleForSRVs = Graphics::CopySRVsToDescriptorHeapAndGetGPUDescriptorHandle(textureSRVsBySlot[i], 1);
		else
			Graphics::CopySRVsToDescriptorHeapAndGetGPUDescriptorHandle(textureSRVsBySlot[i], 1);
	}

	finalized = true;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
	this->colorTint = colorTint;
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
