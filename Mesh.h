#pragma once
#include <d3d12.h>
#include <wrl/client.h> 
#include "Vertex.h"

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

public:
	Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexBuffer();
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();

	Microsoft::WRL::ComPtr<ID3D12Resource> GetIndexBuffer();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

	unsigned int GetIndexCount();
	unsigned int GetVertexCount();
	void Draw();

	Mesh(Vertex* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount);
	Mesh(const char* objFile);

	~Mesh();
};

