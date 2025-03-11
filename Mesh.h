#pragma once
#include <d3d12.h>
#include <wrl/client.h> 
#include "Vertex.h"

struct MeshRaytracingData
{
	D3D12_GPU_DESCRIPTOR_HANDLE IndexBufferSRV{ };
	D3D12_GPU_DESCRIPTOR_HANDLE VertexBufferSRV{ };
	Microsoft::WRL::ComPtr<ID3D12Resource> BLAS;
	unsigned int HitGroupIndex = 0;
};

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW ibView{};

	unsigned int indexCount;
	unsigned int vertexCount;
	//Microsoft::WRL::ComPtr<ID3D11DeviceContext>	context;

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	void CreateBuffers(Vertex* vertices, unsigned int* indices);

	MeshRaytracingData raytracingData;

public:
	Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexBuffer();
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();

	Microsoft::WRL::ComPtr<ID3D12Resource> GetIndexBuffer();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

	MeshRaytracingData GetRaytracingData() { return raytracingData; }

	unsigned int GetVertexCount();
	unsigned int GetIndexCount();
	void Draw();

	Mesh(Vertex* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount);
	Mesh(const char* objFile);

	~Mesh();
};

