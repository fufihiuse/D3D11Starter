#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"
#include "RayTracing.h"

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Create camera
	camera = std::make_shared<Camera>(
		Camera(
			Window::AspectRatio(),							// Aspect Ratio
			DirectX::XMFLOAT3(0, 0, -10),					// Initial Position
			45.0f,											// FOV
			0.01f,											// Near Plane
			1000.0f,										// Far Plane
			0.5f,											// Movement speed
			0.02f,											// Mouse Look Speed
			true											// Perspective Matrix
		));

	CreateRootSigAndPipelineState();

	// Create textures
	materials.push_back(std::make_unique<Material>(pipelineState));
	materials[0]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/cobblestone_albedo.png").c_str()), 0);
	materials[0]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/cobblestone_normals.png").c_str()), 1);
	materials[0]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/cobblestone_roughness.png").c_str()), 2);
	materials[0]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/cobblestone_metal.png").c_str()), 3);
	materials[0]->FinalizeMaterial();

	materials.push_back(std::make_unique<Material>(pipelineState));
	materials[1]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_albedo.png").c_str()), 0);
	materials[1]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_normals.png").c_str()), 1);
	materials[1]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_roughness.png").c_str()), 2);
	materials[1]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_metal.png").c_str()), 3);
	materials[1]->FinalizeMaterial();

	materials.push_back(std::make_unique<Material>(pipelineState));
	materials[2]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/bronze_albedo.png").c_str()), 0);
	materials[2]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/bronze_normals.png").c_str()), 1);
	materials[2]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/bronze_roughness.png").c_str()), 2);
	materials[2]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/bronze_metal.png").c_str()), 3);
	materials[2]->FinalizeMaterial();

	CreateGeometry();
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// Wait for the GPU before we shut down
	Graphics::WaitForGPU();
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Load meshes
	meshes.push_back(std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Models/sphere.obj")).c_str()));
	meshes.push_back(std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Models/helix.obj")).c_str()));
	meshes.push_back(std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Models/cube.obj")).c_str()));

	// Create a BLAS for a single mesh, then the TLAS for our “scene”
	RayTracing::CreateBottomLevelAccelerationStructureForMesh(meshes[0].get());
	RayTracing::CreateTopLevelAccelerationStructureForScene();

	// Finalize any initialization and wait for the GPU
	// before proceeding to the game loop
	Graphics::CloseAndExecuteCommandList();
	Graphics::WaitForGPU();
	Graphics::ResetAllocatorAndCommandList(Graphics::SwapChainIndex());

	// Create entities
	entities.push_back(Entity(meshes[0], camera, materials[0]));

	entities.push_back(Entity(meshes[1], camera, materials[1]));
	entities[1].GetTransform()->SetPosition(2.5, 0, 0);

	entities.push_back(Entity(meshes[2], camera, materials[2]));
	entities[2].GetTransform()->SetPosition(-2.5, 0, 0);

	// Create lights
	lights[0].Type = LIGHT_TYPE_DIR;
	lights[0].Direction = { 1, -1, 0 };
	lights[0].Color = { 1, 1, 1 };
	lights[0].Intensity = 1.0f;

	lights[1].Type = LIGHT_TYPE_DIR;
	lights[1].Direction = { 1, 0, 0 };
	lights[1].Color = { 1, 1, 1 };
	lights[1].Intensity = 1.0f;

	lights[2].Type = LIGHT_TYPE_DIR;
	lights[2].Direction = { 0, 1, 1 };
	lights[2].Color = { 1, 1, 1 };
	lights[2].Intensity = 1.0f;

	lights[3].Type = LIGHT_TYPE_POINT;
	lights[3].Position = { 0, 10, 0 };
	lights[3].Color = { 1, 1, 1 };
	lights[3].Intensity = 1.0f;
	lights[3].Range = 20.0f;

	lights[4].Type = LIGHT_TYPE_POINT;
	lights[4].Position = { -2.5, 0, 0 };
	lights[4].Color = { 1, 1, 1 };
	lights[4].Intensity = 1.0f;
	lights[4].Range = 5.0f;
}

// --------------------------------------------------------
// Loads the two basic shaders, then creates the root signature 
// and pipeline state object for our very basic demo.
// --------------------------------------------------------
void Game::CreateRootSigAndPipelineState()
{
	/*
	// Blobs to hold raw shader byte code used in several steps below
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderByteCode;

	// Load shaders
	{
		// Read our compiled vertex shader code into a blob
		// - Essentially just "open the file and plop its contents here"
		D3DReadFileToBlob(
			FixPath(L"VertexShader.cso").c_str(), vertexShaderByteCode.GetAddressOf());
		D3DReadFileToBlob(
			FixPath(L"PixelShader.cso").c_str(), pixelShaderByteCode.GetAddressOf());
	}

	// Input layout
	const unsigned int inputElementCount = 4;
	D3D12_INPUT_ELEMENT_DESC inputElements[inputElementCount] = {};
	{
		inputElements[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // R32 G32 B32 = float3
		inputElements[0].SemanticName = "POSITION";            // Name must match semantic in shader
		inputElements[0].SemanticIndex = 0;                    // This is the first POSITION semantic

		inputElements[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;    // R32 G32 = float2
		inputElements[1].SemanticName = "TEXCOORD";
		inputElements[1].SemanticIndex = 0;                    // This is the first TEXCOORD semantic

		inputElements[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT; // R32 G32 B32 = float3
		inputElements[2].SemanticName = "NORMAL";
		inputElements[2].SemanticIndex = 0;                    // This is the first NORMAL semantic

		inputElements[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[3].Format = DXGI_FORMAT_R32G32B32_FLOAT; // R32 G32 B32 = float3
		inputElements[3].SemanticName = "TANGENT";
		inputElements[3].SemanticIndex = 0;                    // This is the first TANGENT semantic
	}

	// Root Signature
	{
		// Describe the range of CBVs needed for the vertex shader
		D3D12_DESCRIPTOR_RANGE cbvRangeVS = {};
		cbvRangeVS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvRangeVS.NumDescriptors = 1;
		cbvRangeVS.BaseShaderRegister = 0;
		cbvRangeVS.RegisterSpace = 0;
		cbvRangeVS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Describe the range of CBVs needed for the pixel shader
		D3D12_DESCRIPTOR_RANGE cbvRangePS = {};
		cbvRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvRangePS.NumDescriptors = 1;
		cbvRangePS.BaseShaderRegister = 0;
		cbvRangePS.RegisterSpace = 0;
		cbvRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Create a range of SRV's for textures
		D3D12_DESCRIPTOR_RANGE srvRange = {};
		srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRange.NumDescriptors = 4;	// Set to max number of textures at once (match pixel shader!)
		srvRange.BaseShaderRegister = 0;	// Starts at s0 (match pixel shader!)
		srvRange.RegisterSpace = 0;
		srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Create the root parameters
		D3D12_ROOT_PARAMETER rootParams[3] = {};

		// CBV table param for vertex shader
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[0].DescriptorTable.pDescriptorRanges = &cbvRangeVS;

		// CBV table param for pixel shader
		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[1].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[1].DescriptorTable.pDescriptorRanges = &cbvRangePS;

		// SRV table param
		rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[2].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[2].DescriptorTable.pDescriptorRanges = &srvRange;

		// Create a single static sampler (available to all pixel shaders at the same slot)
		D3D12_STATIC_SAMPLER_DESC anisoWrap = {};
		anisoWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.Filter = D3D12_FILTER_ANISOTROPIC;
		anisoWrap.MaxAnisotropy = 16;
		anisoWrap.MaxLOD = D3D12_FLOAT32_MAX;
		anisoWrap.ShaderRegister = 0;  // register(s0)
		anisoWrap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		D3D12_STATIC_SAMPLER_DESC samplers[] = { anisoWrap };

		// Describe the full root signature
		D3D12_ROOT_SIGNATURE_DESC rootSig = {};
		rootSig.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSig.NumParameters = ARRAYSIZE(rootParams);
		rootSig.pParameters = rootParams;
		rootSig.NumStaticSamplers = ARRAYSIZE(samplers);
		rootSig.pStaticSamplers = samplers;

		ID3DBlob* serializedRootSig = 0;
		ID3DBlob* errors = 0;

		D3D12SerializeRootSignature(
			&rootSig,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&serializedRootSig,
			&errors);

		// Check for errors during serialization
		if (errors != 0)
		{
			OutputDebugString((wchar_t*)errors->GetBufferPointer());
		}

		// Actually create the root sig
		Graphics::Device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(rootSignature.GetAddressOf()));

	}

	// Pipeline state
	{
		// Describe the pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

		// -- Input assembler related ---
		psoDesc.InputLayout.NumElements = inputElementCount;
		psoDesc.InputLayout.pInputElementDescs = inputElements;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// Overall primitive topology type (triangle, line, etc.) is set here
		// IASetPrimTop() is still used to set list/strip/adj options

		// Root sig
		psoDesc.pRootSignature = rootSignature.Get();

		// -- Shaders (VS/PS) ---
		psoDesc.VS.pShaderBytecode = vertexShaderByteCode->GetBufferPointer();
		psoDesc.VS.BytecodeLength = vertexShaderByteCode->GetBufferSize();
		psoDesc.PS.pShaderBytecode = pixelShaderByteCode->GetBufferPointer();
		psoDesc.PS.BytecodeLength = pixelShaderByteCode->GetBufferSize();

		// -- Render targets ---
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;

		// -- States ---
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		psoDesc.RasterizerState.DepthClipEnable = true;

		psoDesc.DepthStencilState.DepthEnable = true;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask =
			D3D12_COLOR_WRITE_ENABLE_ALL;

		// -- Misc ---
		psoDesc.SampleMask = 0xffffffff;

		// Create the pipe state object
		Graphics::Device->CreateGraphicsPipelineState(
			&psoDesc,
			IID_PPV_ARGS(pipelineState.GetAddressOf()));
	}
	*/

	// Initialize raytracing
	RayTracing::Initialize(
		Window::Width(),
		Window::Height(),
		FixPath(L"RayTracing.cso"));

	// Set up the viewport and scissor rectangle
	{
		// Set up the viewport so we render into the correct
		// portion of the render target
		viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)Window::Width();
		viewport.Height = (float)Window::Height();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		// Define a scissor rectangle that defines a portion of
		// the render target for clipping.  This is different from
		// a viewport in that it is applied after the pixel shader.
		// We need at least one of these, but we're rendering to 
		// the entire window, so it'll be the same size.
		scissorRect = {};
		scissorRect.left = 0;
		scissorRect.top = 0;
		scissorRect.right = Window::Width();
		scissorRect.bottom = Window::Height();
	}
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Resize the viewport and scissor rectangle
	// Set up the viewport so we render into the correct
	// portion of the render target
	viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)Window::Width();
	viewport.Height = (float)Window::Height();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Define a scissor rectangle that defines a portion of
	// the render target for clipping.  This is different from
	// a viewport in that it is applied after the pixel shader.
	// We need at least one of these, but we're rendering to 
	// the entire window, so it'll be the same size.
	scissorRect = {};
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = Window::Width();
	scissorRect.bottom = Window::Height();

	// Resize raytracing output texture
	RayTracing::ResizeOutputUAV(Window::Width(), Window::Height());

	// Update matrices
	if (camera == nullptr) return;
	camera->UpdateProjectionMatrix(Window::AspectRatio());
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	entities[0].GetTransform()->Rotate(0, 0, deltaTime * 2);
	entities[1].GetTransform()->MoveAbsolute(0, -.025f * deltaTime, 0);
	entities[2].GetTransform()->SetScale(1, 1, (float)abs(1 + sin(totalTime)));

	camera->Update(deltaTime);
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Grab the current back buffer for this frame
	Microsoft::WRL::ComPtr<ID3D12Resource> currentBackBuffer =
		Graphics::BackBuffers[Graphics::SwapChainIndex()];

	// Perform ray trace (which also copies the results to the back buffer)
	RayTracing::Raytrace(camera, currentBackBuffer);

	// Present
	{
		// Must occur BEFORE present
		Graphics::CloseAndExecuteCommandList();

		// Present the current back buffer and move to the next one
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);
		Graphics::AdvanceSwapChainIndex();

		// Reset allocator & cmd list for next frame
		//Graphics::WaitForGPU();
		Graphics::ResetAllocatorAndCommandList(Graphics::SwapChainIndex());
	}
}



