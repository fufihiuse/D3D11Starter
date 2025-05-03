#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"

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
	// Initialize raytracing
	RayTracing::Initialize(
		Window::Width(),
		Window::Height(),
		FixPath(L"RayTracing.cso"));

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

	// Create textures
	materials.push_back(std::make_unique<Material>(pipelineState));
	materials[0]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/cobblestone_albedo.png").c_str()), 0);
	materials[0]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/cobblestone_normals.png").c_str()), 1);
	materials[0]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/cobblestone_roughness.png").c_str()), 2);
	materials[0]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/cobblestone_metal.png").c_str()), 3);
	materials[0]->SetColorTint(DirectX::XMFLOAT3(.5f, 0, 0));
	materials[0]->FinalizeMaterial();

	materials.push_back(std::make_unique<Material>(pipelineState));
	materials[1]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_albedo.png").c_str()), 0);
	materials[1]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_normals.png").c_str()), 1);
	materials[1]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_roughness.png").c_str()), 2);
	materials[1]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_metal.png").c_str()), 3);
	materials[1]->SetColorTint(DirectX::XMFLOAT3(.25f, .3f, 0));
	materials[1]->FinalizeMaterial();

	materials.push_back(std::make_unique<Material>(pipelineState));
	materials[2]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/bronze_albedo.png").c_str()), 0);
	materials[2]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/bronze_normals.png").c_str()), 1);
	materials[2]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/bronze_roughness.png").c_str()), 2);
	materials[2]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/bronze_metal.png").c_str()), 3);
	materials[2]->SetColorTint(DirectX::XMFLOAT3(0, .3f, .33f));
	materials[2]->FinalizeMaterial();

	materials.push_back(std::make_unique<Material>(pipelineState));
	materials[3]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_albedo.png").c_str()), 0);
	materials[3]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_normals.png").c_str()), 1);
	materials[3]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_roughness.png").c_str()), 2);
	materials[3]->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/Textures/wood_metal.png").c_str()), 3);
	materials[3]->SetColorTint(DirectX::XMFLOAT3(.5f, .7f, 0.4f));
	materials[3]->FinalizeMaterial();

	CreateGeometry();

	RayTracing::CreateTopLevelAccelerationStructureForScene(entities);

	// Finalize any initialization and wait for the GPU
	// before proceeding to the game loop
	Graphics::CloseAndExecuteCommandList();
	Graphics::WaitForGPU();
	Graphics::ResetAllocatorAndCommandList(0);
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

	// Create entities
	entities.push_back(Entity(meshes[0], camera, materials[0]));

	entities.push_back(Entity(meshes[1], camera, materials[1]));
	entities[1].GetTransform()->SetPosition(2.5, 0, 0);

	entities.push_back(Entity(meshes[2], camera, materials[2]));
	entities[2].GetTransform()->SetPosition(-2.5, 0, 0);

	entities.push_back(Entity(meshes[1], camera, materials[3]));
	entities[3].GetTransform()->SetPosition(5, 4, 0);

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
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Update matrices
	if (camera == nullptr) return;
	camera->UpdateProjectionMatrix(Window::AspectRatio());

	// Resize raytracing output texture
	RayTracing::ResizeOutputUAV(Window::Width(), Window::Height());
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

	RayTracing::CreateTopLevelAccelerationStructureForScene(entities);

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



