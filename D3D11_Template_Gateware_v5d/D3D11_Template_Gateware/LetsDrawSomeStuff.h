// This file can be used to contain very basic DX11 Loading, Drawing & Clean Up. (Start Here, or Make your own set of classes)
#pragma once
// Include our DX11 middle ware
#include "Gateware Redistribution R5d/Interface/G_Graphics/GDirectX11Surface.h"
#include <map>
// Include DirectX11 for interface access

#include "Mesh.h" //Mesh.h has all the includes for DirectX
#include "Camera.h"
#include "XTime.h"
#include "BasicVertexShader.csh"
#include "BasicPixelShader.csh"
#include "GridVertexShader.csh"
#include "SkyboxVS.csh"
#include "SkyboxPS.csh"
#include "InstanceVS.csh"
#include "NoLightsPS.csh"
#include "TintPS.csh"
#include <thread>

#define DRAW_BANSHEE 0
// Simple Container class to make life easier/cleaner
using namespace DirectX;

struct MeshConstantBuffer
{
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4 color;
	int enableTexture;
	float time;
	int dir;
	int z;
};

struct LightConstantBuffer
{
	XMFLOAT4 lightDirection[2];
	XMFLOAT4 lightColor[2];

	XMFLOAT4 pointlightPos;
	XMFLOAT4 pointlightColor;
	
	XMFLOAT4 spotlightPos;
	XMFLOAT4 spotlightConeDir;
	XMFLOAT4 spotlightColor;
};

struct CameraConstantBuffer
{
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;
};


// use normals for light saber (dot product)

class LetsDrawSomeStuff
{
	// variables here
	GW::GRAPHICS::GDirectX11Surface* mySurface = nullptr;
	// Gettting these handles from GDirectX11Surface will increase their internal refrence counts, be sure to "Release()" them when done!
	ID3D11Device *myDevice = nullptr;
	IDXGISwapChain *mySwapChain = nullptr;
	ID3D11DeviceContext *myContext = nullptr;

	// TODO: Add your own D3D11 variables here (be sure to "Release()" them when done!)
	CComPtr<ID3D11PixelShader> basicPS = nullptr;
	CComPtr<ID3D11VertexShader> basicVS = nullptr;
	CComPtr<ID3D11VertexShader> gridVS = nullptr;
	CComPtr<ID3D11VertexShader> instanceVS = nullptr;
	CComPtr<ID3D11PixelShader> tintPS = nullptr;
	CComPtr<ID3D11VertexShader> skyboxVS = nullptr;
	CComPtr<ID3D11PixelShader> skyboxPS = nullptr;
	CComPtr<ID3D11PixelShader> noLightsPS = nullptr;

	CComPtr<ID3D11InputLayout> inputLayout = nullptr;
	CComPtr<ID3D11InputLayout> instInputLayout = nullptr;

	CComPtr<ID3D11Buffer> cameraConstBuff = nullptr;
	CComPtr<ID3D11Buffer> meshConstBuff = nullptr;
	CComPtr<ID3D11Buffer> lightConstBuff = nullptr;
	CComPtr<ID3D11Buffer> pixelConstBuff = nullptr;

	CComPtr<ID3D11BlendState> blendState = nullptr;
	CComPtr<ID3D11RasterizerState> CCWcullingMode;
	CComPtr<ID3D11RasterizerState> CWcullingMode;
	CComPtr<ID3D11RasterizerState> NoCullingMode;
	CComPtr<ID3D11DepthStencilState> DSLessEqual;
	CComPtr<ID3D11Texture2D> sparkTexture;

	D3D11_VIEWPORT viewPort[2];

	XMFLOAT4X4 secondProject;
	XMFLOAT4X4 secondView;

	Mesh gruntTorso;
	Mesh gruntRightSide;
	Mesh gruntLeftSide;
	Mesh odst;
	Mesh shotgun;
	Mesh smg;
	Mesh sparks;

	Mesh spartan;
	Mesh banshee;
	Mesh pelican;

	Mesh transparentGreenCube;
	Mesh transparentRedCube;
	Mesh transparentBlueCube;

	Mesh grid;
	Mesh hammer;
	Mesh needler;
	Mesh oddball;

	Mesh baseMap;
	Mesh ring;
	Mesh ground;
	Mesh skybox;
	Mesh renderToCube;

	Mesh* currentObject = &banshee;

	Camera mainCamera;

	XMFLOAT4 gruntSpawnpoints[5];
	XMFLOAT4 gruntColors[5];

	bool updateCameraBuffer;
	bool turnOnSecondLight;

	float gridTimer = 0;

	bool gridDirection = true;

public:

	XTime timer;
	// Init
	LetsDrawSomeStuff(GW::SYSTEM::GWindow* attatchPoint);
	// Shutdown
	~LetsDrawSomeStuff();
	// Draw
	void Render();

	void ManageUserInput();

	void UpdateProjection();
	
	void InitializeGruntData();
};

// Init
LetsDrawSomeStuff::LetsDrawSomeStuff(GW::SYSTEM::GWindow* attatchPoint)
{
	if (attatchPoint) // valid window?
	{
		// Create surface, will auto attatch to GWindow
		if (G_SUCCESS(GW::GRAPHICS::CreateGDirectX11Surface(attatchPoint, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT, &mySurface)))
		{
			updateCameraBuffer = true;
			turnOnSecondLight = true;

			// Grab handles to all DX11 base interfaces
			mySurface->GetDevice((void**)&myDevice);
			mySurface->GetSwapchain((void**)&mySwapChain);
			mySurface->GetContext((void**)&myContext);

			// TODO: Create new DirectX stuff here! (Buffers, Shaders, Layouts, Views, Textures, etc...)

			InitializeGruntData();

			#pragma region Camera

			float aspectRatio;
			mySurface->GetAspectRatio(aspectRatio);

			mainCamera.InitializeCamera(aspectRatio);

			#pragma endregion


			#pragma region Meshes

			std::vector<std::thread> meshes;
			
			XMFLOAT4X4 tempWorldMatrix;

			//Grid
			grid.InitializeAs3DGrid(myDevice);
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(0,4.5f,0));
			grid.SetWorldMatrix(tempWorldMatrix);


			//Mjölnir
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(0,-60,0));
			hammer.SetWorldMatrix(tempWorldMatrix);
			hammer.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/HammerTexture.dds");
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &hammer, myDevice, "../D3D11_Template_Gateware/Models/ThorHammer.obj", false));

			//Skybox
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixScaling(10, 10, 10));
			skybox.SetWorldMatrix(tempWorldMatrix);
			skybox.CreateSkybox(myDevice, myContext, "../D3D11_Template_Gateware/Models/SkyboxTest.obj", L"../D3D11_Template_Gateware/Models/StarsSkybox.dds");

			//Base map
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixScaling(.5f,.5f,.5f));
			baseMap.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &baseMap, myDevice, "../D3D11_Template_Gateware/Models/BaseMap.obj", false));
			baseMap.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/BeamTexture.dds");
			//Ring 
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() *  XMMatrixTranslation(0, -50, 0) );

			ring.SetWorldMatrix(tempWorldMatrix);
			ring.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/RingTexture.dds");
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &ring, myDevice, "../D3D11_Template_Gateware/Models/Ring.obj", false));

			//Ground 
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixScaling(.865f, 1, .9f) *  XMMatrixTranslation(0, -50, 0));
			ground.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/GrassTexture.dds");
			ground.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &ground, myDevice, "../D3D11_Template_Gateware/Models/Ground.obj", TRUE));
			


			//Sparks

			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixScaling(1.5f, 1.5f, 1.5f));
			sparks.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &sparks, myDevice, "../D3D11_Template_Gateware/Models/Sparks.obj", false));
			sparks.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/SparksTexture.dds");

			//Grunt
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity());
			gruntTorso.SetWorldMatrix(tempWorldMatrix);
			gruntRightSide.SetWorldMatrix(tempWorldMatrix);
			gruntLeftSide.SetWorldMatrix(tempWorldMatrix);


			std::map<int, InstanceType> instanceMap;
			
			while(instanceMap.size() < 3) 
			{
				int i = rand() % 5;
				int c = rand() % 5;
				
				InstanceType inst = { gruntSpawnpoints[i], gruntColors[c]};

				if(instanceMap.count(i) == 0)
				{
					instanceMap.insert({ i, inst });
				}
			}

			InstanceType instData[3];
			std::map<int, InstanceType>::iterator i = instanceMap.begin();
			int index = 0;
			for(i; i != instanceMap.end(); i++)
			{
				instData[index] = i->second;

				instData[index].posAndRot.w = rand() % 360 + 1;
				index++;
			}

			gruntTorso.SetInstancingData(3, instData);
			gruntRightSide.SetInstancingData(3, instData);
			gruntLeftSide.SetInstancingData(3, instData);



			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &gruntTorso, myDevice, "../D3D11_Template_Gateware/Models/GruntTorso.obj", false));
			gruntTorso.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/GruntTorsoTexture.dds");

			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &gruntRightSide, myDevice, "../D3D11_Template_Gateware/Models/GruntRightArmLeg.obj", false));
			gruntRightSide.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/GruntLegsArmsTexture.dds");

			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &gruntLeftSide, myDevice, "../D3D11_Template_Gateware/Models/GruntLeftArmLeg.obj", false));
			gruntLeftSide.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/GruntLegsArmsTexture.dds");

			//Spartan
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(2, -60, 0));
			spartan.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &spartan, myDevice, "../D3D11_Template_Gateware/Models/Spartan.obj", false));
			spartan.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/SpartanTexture.dds");

			//ODST
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixScaling(.15f, .15f, .15f)  *  XMMatrixRotationY(90) * XMMatrixTranslation(-20, -18.5f, -25));
			odst.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &odst, myDevice, "../D3D11_Template_Gateware/Models/ODST.obj", false));
			odst.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/ODSTTexture.dds");

			//Shotgun
			shotgun.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &shotgun, myDevice, "../D3D11_Template_Gateware/Models/Shotgun.obj", false));
			shotgun.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/ShotgunTexture.dds");

			//Smg
			smg.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &smg, myDevice, "../D3D11_Template_Gateware/Models/Smg.obj", false));
			smg.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/SmgTexture.dds");

			//Transparent cubes
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(0, -60, 8) * XMMatrixScaling(2.0f, 1, 0.2f));
			transparentRedCube.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &transparentRedCube, myDevice, "../D3D11_Template_Gateware/Models/Cube.obj", false));
			transparentRedCube.baseColor = XMFLOAT4(1, 0, 0, 1);

			tempWorldMatrix = MatrixStorage(MatrixRegister(tempWorldMatrix) * XMMatrixTranslation(0, 0, 1));
			transparentGreenCube.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &transparentGreenCube, myDevice, "../D3D11_Template_Gateware/Models/Cube.obj", false));
			transparentGreenCube.baseColor = XMFLOAT4(0, 1, 0, 1);

			tempWorldMatrix = MatrixStorage(MatrixRegister(tempWorldMatrix) * XMMatrixTranslation(0, 0, 1));
			transparentBlueCube.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &transparentBlueCube, myDevice, "../D3D11_Template_Gateware/Models/Cube.obj", false));
			transparentBlueCube.baseColor = XMFLOAT4(0, 0, 1, 1);

			//RenderToCube
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(-5, -40, -5) * XMMatrixScaling(2.0f, 2.0f, 2.0f));
			renderToCube.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &renderToCube, myDevice, "../D3D11_Template_Gateware/Models/Cube.obj", false));
			renderToCube.CreateTextureManually(myDevice);

			//Neddler 
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity());
			needler.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &needler, myDevice, "../D3D11_Template_Gateware/Models/Needler.obj", false));

			#if DRAW_BANSHEE 
			//Banshee
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(-60, 2,0));
			banshee.SetWorldMatrix(tempWorldMatrix);
			banshee.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/BansheeTexture.dds");
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &banshee, myDevice, "../D3D11_Template_Gateware/Models/Banshee2.obj", false));
	
			#endif

			//Pelican
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity()* XMMatrixRotationY(40) * XMMatrixTranslation(65, 2, 0) );
			pelican.SetWorldMatrix(tempWorldMatrix);
			pelican.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/PelicanTexture.dds");
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &pelican, myDevice, "../D3D11_Template_Gateware/Models/Pelican.obj", false));

			//Oddball
			tempWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixScaling(.1f, .1f, .1f) * XMMatrixTranslation(-19, 10, -25));
			oddball.SetWorldMatrix(tempWorldMatrix);
			meshes.push_back(std::thread(&Mesh::LoadMeshFromFile, &oddball, myDevice, "../D3D11_Template_Gateware/Models/Oddball.obj", false));
			oddball.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/OddballTexture.dds");

			for(auto& i : meshes)
			{
				i.join();
			}

			#pragma endregion 
			

			#pragma region Sampler

			D3D11_SAMPLER_DESC desc = { };
			ZeroMemory(&desc, sizeof(desc));

			desc.Filter = D3D11_FILTER_ANISOTROPIC;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MinLOD = 0;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			
			myDevice->CreateSamplerState(&desc, &hammer.m_pSamplerState.p);


			myDevice->CreateSamplerState(&desc, &skybox.m_pSamplerState.p);
			#pragma endregion


			#pragma region Shaders

			myDevice->CreateVertexShader(BasicVertexShader, sizeof(BasicVertexShader), nullptr, &basicVS.p);
			myDevice->CreateVertexShader(GridVertexShader, sizeof(GridVertexShader), nullptr, &gridVS.p);
			myDevice->CreateVertexShader(SkyboxVS, sizeof(SkyboxVS), nullptr, &skyboxVS.p);
			myDevice->CreateVertexShader(InstanceVS, sizeof(InstanceVS), nullptr, &instanceVS.p);

			myDevice->CreatePixelShader(SkyboxPS, sizeof(SkyboxPS), nullptr, &skyboxPS.p);
			myDevice->CreatePixelShader(BasicPixelShader, sizeof(BasicPixelShader), nullptr, &basicPS.p);
			myDevice->CreatePixelShader(NoLightsPS, sizeof(NoLightsPS), nullptr,&noLightsPS.p);
			myDevice->CreatePixelShader(TintPS, sizeof(TintPS), nullptr, &tintPS.p);
			#pragma endregion


			#pragma region Input Layout

			D3D11_INPUT_ELEMENT_DESC layout[]
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXTURECOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			myDevice->CreateInputLayout(layout, 3, BasicVertexShader, sizeof(BasicVertexShader), &inputLayout.p);

			D3D11_INPUT_ELEMENT_DESC instLayout[]
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXTURECOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

				{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{ "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}

			};

			myDevice->CreateInputLayout(instLayout, 5, InstanceVS, sizeof(InstanceVS), &instInputLayout.p);
			#pragma endregion


			#pragma region  Buffers

			D3D11_BUFFER_DESC buffDesc = { 0 };
			buffDesc.Usage = D3D11_USAGE_DYNAMIC;
			buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffDesc.ByteWidth = sizeof(MeshConstantBuffer);
			buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			myDevice->CreateBuffer(&buffDesc, nullptr, &meshConstBuff.p);

			buffDesc.ByteWidth = sizeof(CameraConstantBuffer);
			myDevice->CreateBuffer(&buffDesc, nullptr, &cameraConstBuff.p);

			buffDesc.ByteWidth = sizeof(LightConstantBuffer);
			myDevice->CreateBuffer(&buffDesc, nullptr, &lightConstBuff.p);

			#pragma endregion


			#pragma region  Blending

			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
			
			D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc;
			ZeroMemory(&rtBlendDesc, sizeof(rtBlendDesc));

			rtBlendDesc.BlendEnable = true;
			rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_COLOR;
			rtBlendDesc.DestBlend = D3D11_BLEND_BLEND_FACTOR;
			rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
			rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
			rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
			rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rtBlendDesc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;


			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0] = rtBlendDesc;

			myDevice->CreateBlendState(&blendDesc, &blendState);


			D3D11_RASTERIZER_DESC rastDesc;
			ZeroMemory(&rastDesc, sizeof(rastDesc));

			rastDesc.FillMode = D3D11_FILL_SOLID;
			rastDesc.CullMode = D3D11_CULL_BACK;

			rastDesc.FrontCounterClockwise = false;
			myDevice->CreateRasterizerState(&rastDesc, &CWcullingMode.p);

			rastDesc.FrontCounterClockwise = true;
			myDevice->CreateRasterizerState(&rastDesc, &CCWcullingMode.p);

			//Skybox stuff

			ZeroMemory(&rastDesc, sizeof(rastDesc));
			rastDesc.FillMode = D3D11_FILL_SOLID;
			rastDesc.CullMode = D3D11_CULL_NONE;

			myDevice->CreateRasterizerState(&rastDesc, &NoCullingMode.p);
			
			D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			myDevice->CreateDepthStencilState(&dsDesc, &DSLessEqual.p);
			#pragma endregion


			secondView  = MatrixStorage(XMMatrixTranslation(0, 0, -5));

			viewPort[0].TopLeftX = 0;
			viewPort[0].TopLeftY = 0;
			viewPort[0].Width = 250;
			viewPort[0].Height = 250;
			viewPort[0].MaxDepth = 1;
			viewPort[0].MinDepth = 0;


			viewPort[1].TopLeftX = 0;
			viewPort[1].TopLeftY = 0;

			DXGI_SWAP_CHAIN_DESC scDesc;
			mySwapChain->GetDesc(&scDesc);


			viewPort[1].Width = scDesc.BufferDesc.Width;
			viewPort[1].Height = scDesc.BufferDesc.Height;
			viewPort[1].MaxDepth = 1;
			viewPort[1].MinDepth = 0;		

			secondProject = MatrixStorage(XMMatrixPerspectiveFovLH(XMConvertToRadians(90), scDesc.BufferDesc.Width / scDesc.BufferDesc.Height, .1f, 500));

		}
	}
}

// Shutdown
LetsDrawSomeStuff::~LetsDrawSomeStuff()
{
	// Release DX Objects aquired from the surface
	myDevice->Release();
	mySwapChain->Release();
	myContext->Release();
	
	// TODO: "Release()" more stuff here!
	
	if (mySurface) // Free Gateware Interface
	{
		mySurface->DecrementCount(); // reduce internal count (will auto delete on Zero)
		mySurface = nullptr; // the safest way to fly
	}
}

// Draw
void LetsDrawSomeStuff::Render()
{
	if (mySurface) // valid?
	{
		// this could be changed during resolution edits, get it every frame
		ID3D11RenderTargetView *myRenderTargetView = nullptr;
		ID3D11DepthStencilView *myDepthStencilView = nullptr;
		if (G_SUCCESS(mySurface->GetRenderTarget((void**)&myRenderTargetView)))
		{
			// Grab the Z Buffer if one was requested
			if (G_SUCCESS(mySurface->GetDepthStencilView((void**)&myDepthStencilView)))
			{
				myContext->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0); // clear it to Z exponential Far.
				myDepthStencilView->Release();
			}


			// Set active target for drawing, all array based D3D11 functions should use a syntax similar to below
			ID3D11RenderTargetView* const targets[] = { myRenderTargetView };
			myContext->OMSetRenderTargets(1, targets, myDepthStencilView);

			// Clear the screen to black
			const float d_green[] = { 0, 0, 0, 1 };
			myContext->ClearRenderTargetView(myRenderTargetView, d_green);
			
			// TODO: Set your shaders, Update & Set your constant buffers, Attatch your vertex & index buffers, Set your InputLayout & Topology & Draw!
			CameraConstantBuffer ccb = {  };
			D3D11_MAPPED_SUBRESOURCE data = { 0 };
			MeshConstantBuffer mcb = {  };

			//m_pRenderTargetView
			myContext->RSSetViewports(1, &viewPort[1]);
			{

				//Update Camera Buffer


				ccb.viewMatrix = MatrixStorage(XMMatrixInverse(nullptr, MatrixRegister(mainCamera.mViewMatrix)));
				ccb.projMatrix = mainCamera.mProjMatrix;


				myContext->Map(cameraConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(ccb), &ccb, sizeof(ccb));
				myContext->Unmap(cameraConstBuff, 0);

				myContext->VSSetConstantBuffers(0, 1, &cameraConstBuff.p);

				updateCameraBuffer = false;



				XMFLOAT4X4 oddBallMatrix = MatrixStorage(XMMatrixRotationY((float)timer.TotalTime() * 2) * XMMatrixTranslation(0, 0, -50) * XMMatrixRotationY((float)timer.TotalTime() * 2) * MatrixRegister(oddball.GetWorldMatrix()));

				XMVECTOR scale;
				XMVECTOR rot;
				XMVECTOR trans;

				XMMatrixDecompose(&scale, &rot, &trans, MatrixRegister(oddBallMatrix));
				//Update lights

				LightConstantBuffer lcb = {  };
				lcb.lightDirection[0] = XMFLOAT4(0.577f, -0.6f, 0.577f, 1.0f);
				lcb.lightDirection[1] = XMFLOAT4(0, 0, 1, 1.0f);
				lcb.lightColor[0] = XMFLOAT4(0.729f, 0.764f, 0.827f, 0.1f);

				if(turnOnSecondLight)
					lcb.lightColor[1] = XMFLOAT4(0.2f, 0.1f, 0.8f, 0.1f);
				else
					lcb.lightColor[1] = XMFLOAT4(0, 0, 0, 0);

				lcb.pointlightColor = XMFLOAT4(0, 1, 0, 1);

				lcb.spotlightPos = XMFLOAT4(-20, 0, -27, 1);
				lcb.spotlightConeDir = XMFLOAT4(0, -1, 0, 1);
				lcb.spotlightColor = XMFLOAT4(0.3803f, 0.1647f, 0.4475f, 1);

				lcb.lightDirection[1] = VectorStorage(XMVector3Rotate(VectorRegister(lcb.lightDirection[1]), XMQuaternionRotationMatrix(XMMatrixRotationY((float)timer.TotalTime() * 2))));

				lcb.pointlightPos = VectorStorage(trans);

				data = { 0 };
				myContext->Map(lightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(lcb), &lcb, sizeof(lcb));
				myContext->Unmap(lightConstBuff, 0);

				myContext->PSSetConstantBuffers(0, 1, &lightConstBuff.p);

				//Mjölnir 
				mcb.worldMatrix = hammer.GetWorldMatrix();
				mcb.enableTexture = 1;
				mcb.time = (float)timer.TotalTime();
				mcb.dir = gridDirection;

				gridTimer += (float)timer.Delta();

				if(gridTimer > 1)
				{
					mcb.dir = !mcb.dir;
					gridTimer = 0;
				}

				data = { 0 };
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				myContext->VSSetConstantBuffers(1, 1, &meshConstBuff.p);

				hammer.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				XMVECTOR lightPosition = VectorRegister(lcb.lightDirection[1]);
				lightPosition *= VectorRegister(XMFLOAT4(3, 1, 3, 1));

				XMFLOAT4X4 lightMatrix = ( MatrixStorage(XMMatrixScaling(.2f, .2f, .2f) * XMMatrixTranslationFromVector(-lightPosition * 5) * XMMatrixTranslation(0, 5, 0)) );



				//Grunt
				mcb.color = XMFLOAT4(.8f, 0, 0.5f, 1.0f);
				mcb.worldMatrix = gruntTorso.GetWorldMatrix();
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				gruntTorso.RenderInstancesOfMesh(myContext, instanceVS.p, tintPS.p, instInputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				gruntRightSide.RenderInstancesOfMesh(myContext, instanceVS.p, tintPS.p, instInputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				gruntLeftSide.RenderInstancesOfMesh(myContext, instanceVS.p, tintPS.p, instInputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				//Spartan
				mcb.worldMatrix = spartan.GetWorldMatrix();
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				spartan.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		

				//Ring

				mcb.worldMatrix = ring.GetWorldMatrix();
				mcb.color = XMFLOAT4(1, 0, 0, 1);
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				ring.RenderMesh(myContext, basicVS.p, noLightsPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

#if DRAW_BANSHEE

				mcb.worldMatrix = banshee.GetWorldMatrix();
				mcb.color = XMFLOAT4(1, 0, 0, 1);
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				banshee.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

#endif

				//ODST
				mcb.worldMatrix = odst.GetWorldMatrix();
				mcb.color = XMFLOAT4(1, 0, 0, 1);
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				odst.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				smg.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				shotgun.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


				//Oddball

				mcb.worldMatrix = oddBallMatrix;
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				oddball.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				//Pelican
				data = { 0 };
				mcb.color = XMFLOAT4(0.105f, 0.113f, 0.109f, 1);
				mcb.worldMatrix = pelican.GetWorldMatrix();
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);


				pelican.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				//Base map
				mcb.color = XMFLOAT4(0.105f, 0.113f, 0.109f, 1);
				mcb.worldMatrix = baseMap.GetWorldMatrix();
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				baseMap.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


				//Ground
				data = { 0 };
				mcb.color = XMFLOAT4(0.105f, 0.113f, 0.109f, 1);
				mcb.worldMatrix = ground.GetWorldMatrix();
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);


				ground.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				//No texture


				// Third Mjolnir
				lightMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixScaling(.2f, .2f, .2f));

				lightMatrix._41 = lcb.spotlightPos.x;
				lightMatrix._42 = lcb.spotlightPos.y;
				lightMatrix._43 = lcb.spotlightPos.z;

				data = { 0 };
				mcb.color = XMFLOAT4(0, 1, 1, 1);
				mcb.enableTexture = 0;
				mcb.worldMatrix = lightMatrix;
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				myContext->VSSetConstantBuffers(1, 1, &meshConstBuff.p);

				hammer.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				

				//Grid
				mcb.color = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
				mcb.worldMatrix = grid.GetWorldMatrix();
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				grid.TestGrid(myContext, gridVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

				//Transparent objects

				float blendFactor[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
				myContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);

				//Distance check 

				std::vector<Mesh*> transparentObjects;
				transparentObjects.push_back(&transparentRedCube);
				transparentObjects.push_back(&transparentGreenCube);
				transparentObjects.push_back(&transparentBlueCube);

				std::map<float, int> depthList;

				XMVECTOR cameraPos;
				XMVECTOR cameraRot;
				XMVECTOR cameraScale;

				XMMatrixDecompose(&cameraScale, &cameraRot, &cameraPos, MatrixRegister(mainCamera.mViewMatrix));

				for(unsigned int i = 0; i < transparentObjects.size(); i++)
				{
					XMVECTOR cubePos;

					XMMatrixDecompose(&cameraScale, &cameraRot, &cubePos, MatrixRegister(transparentObjects[i]->GetWorldMatrix()));

					float distX = XMVectorGetX(cubePos) - XMVectorGetX(cameraPos);
					float distY = XMVectorGetY(cubePos) - XMVectorGetY(cameraPos);
					float distZ = XMVectorGetZ(cubePos) - XMVectorGetZ(cameraPos);


					float totalDist = sqrtf(distX * distX + distY * distY + distZ * distZ);

					depthList.insert({ totalDist, i });
				}

				std::map<float, int>::reverse_iterator it = depthList.rbegin();

				for(it; it != depthList.rend(); it++)
				{
					Mesh* currObject = transparentObjects[it->second];
					mcb.color = currObject->baseColor;
					mcb.worldMatrix = currObject->GetWorldMatrix();
					myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
					memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
					myContext->Unmap(meshConstBuff, 0);

					//Draw back side 
					myContext->RSSetState(CCWcullingMode);
					currObject->RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					//Draw front side
					myContext->RSSetState(CWcullingMode);
					currObject->RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				}
				myContext->OMSetBlendState(0, 0, 0xffffffff);


				// Skybox
				data = { 0 };
				mcb.enableTexture = 0;
				mcb.color = XMFLOAT4(1, 1, 1, 1);
				mcb.worldMatrix = skybox.GetWorldMatrix();
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				myContext->VSSetConstantBuffers(1, 1, &meshConstBuff.p);
				myContext->OMSetDepthStencilState(DSLessEqual, 0);
				//myContext->RSSetState(NoCullingMode);
				skybox.RenderMesh(myContext, skyboxVS.p, skyboxPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				myContext->OMSetDepthStencilState(NULL, 0);

				//myContext->RSSetState(CWcullingMode);

			}

			myContext->RSSetViewports(1, &viewPort[0]);
			{
				ID3D11RenderTargetView* const targets[] = { myRenderTargetView, renderToCube.m_pRenderTargetView.p };

				myContext->OMSetRenderTargets(2, targets, myDepthStencilView);

				ccb.viewMatrix = MatrixStorage(XMMatrixInverse(nullptr, MatrixRegister(secondView)));
				ccb.projMatrix = secondProject;


				myContext->Map(cameraConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(ccb), &ccb, sizeof(ccb));
				myContext->Unmap(cameraConstBuff, 0);

				myContext->VSSetConstantBuffers(0, 1, &cameraConstBuff.p);

				mcb.worldMatrix = MatrixStorage(MatrixRegister(sparks.GetWorldMatrix()) * XMMatrixRotationY(timer.TotalTime()));
				mcb.enableTexture = true;
				mcb.color = XMFLOAT4(1, 0, 0, 1);
				myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
				myContext->Unmap(meshConstBuff, 0);

				sparks.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			}
			
			myContext->OMSetRenderTargets(1, &myRenderTargetView, myDepthStencilView);
			ccb.viewMatrix = mainCamera.mViewMatrix;
			ccb.projMatrix = mainCamera.mProjMatrix;


			myContext->Map(cameraConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(ccb), &ccb, sizeof(ccb));
			myContext->Unmap(cameraConstBuff, 0);

			myContext->VSSetConstantBuffers(0, 1, &cameraConstBuff.p);

			myContext->RSSetViewports(1, &viewPort[1]);

			mcb.worldMatrix = renderToCube.GetWorldMatrix();
			mcb.color = XMFLOAT4(1, 0, 0, 1);
			mcb.enableTexture = true;
			myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
			myContext->Unmap(meshConstBuff, 0);

			renderToCube.RenderMesh(myContext, basicVS.p, basicPS.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Present Backbuffer using Swapchain object
			// Framerate is currently unlocked, we suggest "MSI Afterburner" to track your current FPS and memory usage.
			mySwapChain->Present(0, 0); // set first argument to 1 to enable vertical refresh sync with display

			// Free any temp DX handles aquired this frame
			myRenderTargetView->Release();
			
		}
	}
}

void LetsDrawSomeStuff::ManageUserInput()
{
	float delta = (float)timer.Delta();
	float movementSpeed = 15;
	float rotationSpeed = 2;
	
	if(GetAsyncKeyState('I') & 0x1)
	{
		turnOnSecondLight = !turnOnSecondLight;
	}

	if(GetAsyncKeyState(0x31) & 0x1)
	{
		//pelican
		currentObject = &pelican;
	}

	if(GetAsyncKeyState(0x32) & 0x1)
	{
		//banshee
		currentObject = &banshee;
	}

	if(GetAsyncKeyState(0x33) & 0x1)
	{
		//odst 
		currentObject = &odst;
	}

	if(GetAsyncKeyState(0x34) & 0x1)
	{
		//spartan
		currentObject = &spartan;
	}

	if(GetAsyncKeyState(0x35) & 0x1)
	{
		//grunts
		currentObject = &gruntTorso;
	}

	if(GetAsyncKeyState(0x36) & 0x1)
	{
		//sparks
		currentObject = &sparks;
	}

	//Camera Movement
	if(GetAsyncKeyState('W'))
	{
		mainCamera.mViewMatrix = MatrixStorage( XMMatrixTranslation(0.0f, 0.0f, delta * movementSpeed) * MatrixRegister(mainCamera.mViewMatrix));

		XMVECTOR camPos = XMVectorZero();
		XMVECTOR camRot = XMVectorZero();
		XMVECTOR camScale = XMVectorZero();
		XMMatrixDecompose(&camScale, &camRot, &camPos, MatrixRegister(mainCamera.mViewMatrix));

		skybox.SetWorldMatrix(MatrixStorage(XMMatrixScaling(10, 10, 10) * XMMatrixTranslationFromVector(camPos)));

		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState('A'))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixTranslation(-delta * movementSpeed, 0.0f, 0.0f) * MatrixRegister(mainCamera.mViewMatrix));
	
		XMVECTOR camPos = XMVectorZero();
		XMVECTOR camRot = XMVectorZero();
		XMVECTOR camScale = XMVectorZero();
		XMMatrixDecompose(&camScale, &camRot, &camPos, MatrixRegister(mainCamera.mViewMatrix));

		skybox.SetWorldMatrix(MatrixStorage(XMMatrixScaling(10, 10, 10) * XMMatrixTranslationFromVector(camPos)));

		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState('S'))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixTranslation(0.0f, 0.0f, -delta * movementSpeed) * MatrixRegister(mainCamera.mViewMatrix));

		XMVECTOR camPos = XMVectorZero();
		XMVECTOR camRot = XMVectorZero();
		XMVECTOR camScale = XMVectorZero(); 
		XMMatrixDecompose(&camScale, &camRot, &camPos, MatrixRegister(mainCamera.mViewMatrix));

		skybox.SetWorldMatrix(MatrixStorage(XMMatrixScaling(10, 10, 10) * XMMatrixTranslationFromVector(camPos)));

		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState('D'))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixTranslation(delta * movementSpeed, 0.0f, 0.0f) *  MatrixRegister(mainCamera.mViewMatrix));
		
		XMVECTOR camPos = XMVectorZero();
		XMVECTOR camRot = XMVectorZero();
		XMVECTOR camScale = XMVectorZero();	
		XMMatrixDecompose(&camScale, &camRot, &camPos, MatrixRegister(mainCamera.mViewMatrix));

		skybox.SetWorldMatrix(MatrixStorage(XMMatrixScaling(10, 10, 10) * XMMatrixTranslationFromVector(camPos)));

		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_SPACE))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixTranslation(0.0f, delta * movementSpeed, 0.0f) * MatrixRegister(mainCamera.mViewMatrix));
		
		XMVECTOR camPos = XMVectorZero();
		XMVECTOR camRot = XMVectorZero();
		XMVECTOR camScale = XMVectorZero();
		XMMatrixDecompose(&camScale, &camRot, &camPos, MatrixRegister(mainCamera.mViewMatrix));

		skybox.SetWorldMatrix(MatrixStorage(XMMatrixScaling(10, 10, 10) * XMMatrixTranslationFromVector(camPos)));

		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LCONTROL))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixTranslation(0.0f, -delta * movementSpeed, 0.0f) * MatrixRegister(mainCamera.mViewMatrix));
		
		XMVECTOR camPos = XMVectorZero();
		XMVECTOR camRot = XMVectorZero();
		XMVECTOR camScale = XMVectorZero();
		XMMatrixDecompose(&camScale, &camRot, &camPos, MatrixRegister(mainCamera.mViewMatrix));

		skybox.SetWorldMatrix(MatrixStorage(XMMatrixScaling(10, 10, 10) * XMMatrixTranslationFromVector(camPos)));
		updateCameraBuffer = true;
	}
	
	XMVECTOR objPos;
	XMVECTOR objRot;
	XMVECTOR objScale;

	 

	XMMatrixDecompose(&objScale, &objRot, &objPos, MatrixRegister(currentObject->GetWorldMatrix()));



	//Temp obj Movement
	if(GetAsyncKeyState('Y'))
	{
		currentObject->SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, 0.0f, delta * movementSpeed) *		MatrixRegister(currentObject->GetWorldMatrix())));
		//gruntRightSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, 0.0f, delta * movementSpeed) *	MatrixRegister(gruntRightSide.GetWorldMatrix())));
		//gruntLeftSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, 0.0f, delta * movementSpeed) *		MatrixRegister(gruntLeftSide.GetWorldMatrix())));
	}
	if(GetAsyncKeyState('G'))
	{
		currentObject->SetWorldMatrix(MatrixStorage(XMMatrixTranslation(-delta * movementSpeed, 0.0f, 0.0f) *		MatrixRegister(currentObject->GetWorldMatrix())));
		//gruntLeftSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(-delta * movementSpeed, 0.0f, 0.0f) *	MatrixRegister(gruntLeftSide.GetWorldMatrix())));
		//gruntRightSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(-delta * movementSpeed, 0.0f, 0.0f) *	MatrixRegister(gruntRightSide.GetWorldMatrix())));
	}

	if(GetAsyncKeyState('H'))
	{
		currentObject->SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, 0.0f, -delta * movementSpeed) *		MatrixRegister(currentObject->GetWorldMatrix())));
		//gruntLeftSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, 0.0f, -delta * movementSpeed) *	MatrixRegister(gruntLeftSide.GetWorldMatrix())));
		//gruntRightSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, 0.0f, -delta * movementSpeed) *	MatrixRegister(gruntRightSide.GetWorldMatrix())));
	}

	if(GetAsyncKeyState('J'))
	{
		currentObject->SetWorldMatrix(MatrixStorage(XMMatrixTranslation(delta * movementSpeed, 0.0f, 0.0f) *		MatrixRegister(currentObject->GetWorldMatrix())));
	//	gruntLeftSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(delta * movementSpeed, 0.0f, 0.0f) *		MatrixRegister(gruntLeftSide.GetWorldMatrix())));
		//gruntRightSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(delta * movementSpeed, 0.0f, 0.0f) *	MatrixRegister(gruntRightSide.GetWorldMatrix())));
	}

	if(GetAsyncKeyState('N'))
	{
		currentObject->SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, delta * movementSpeed, 0.0f) *		MatrixRegister(currentObject->GetWorldMatrix())));
		//gruntLeftSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, delta * movementSpeed, 0.0f) *		MatrixRegister(gruntLeftSide.GetWorldMatrix())));
		//gruntRightSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, delta * movementSpeed, 0.0f) *	MatrixRegister(gruntRightSide.GetWorldMatrix())));
	}


	if(GetAsyncKeyState('M'))
	{
		currentObject->SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, -delta * movementSpeed, 0.0f) *		MatrixRegister(currentObject->GetWorldMatrix())));
		//gruntLeftSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, -delta * movementSpeed, 0.0f) *	MatrixRegister(gruntLeftSide.GetWorldMatrix())));
		//gruntRightSide.SetWorldMatrix(MatrixStorage(XMMatrixTranslation(0.0f, -delta * movementSpeed, 0.0f) *	MatrixRegister(gruntRightSide.GetWorldMatrix())));
	}






	//Camera Rotation

	if(GetAsyncKeyState(VK_UP))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixRotationX(-delta * rotationSpeed) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_RIGHT))
	{
		XMVECTOR pos;
		XMVECTOR rot;
		XMVECTOR scale;

		XMMatrixDecompose(&scale, &rot, &pos, MatrixRegister(mainCamera.mViewMatrix));
		
		mainCamera.mViewMatrix._41, mainCamera.mViewMatrix._42, mainCamera.mViewMatrix._43 = 0;

		mainCamera.mViewMatrix = MatrixStorage( MatrixRegister(mainCamera.mViewMatrix) * XMMatrixRotationY(delta * rotationSpeed) );

		XMFLOAT4 hey = VectorStorage(pos);

		mainCamera.mViewMatrix._41 = hey.x;
		mainCamera.mViewMatrix._42 = hey.y;
		mainCamera.mViewMatrix._43 = hey.z;

		updateCameraBuffer = true;								

	}

	if(GetAsyncKeyState(VK_DOWN))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixRotationX(delta * rotationSpeed) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LEFT))
	{
		XMVECTOR pos;
		XMVECTOR rot;
		XMVECTOR scale;

		XMMatrixDecompose(&scale, &rot, &pos, MatrixRegister(mainCamera.mViewMatrix));

		mainCamera.mViewMatrix._41, mainCamera.mViewMatrix._42, mainCamera.mViewMatrix._43 = 0;

		mainCamera.mViewMatrix = MatrixStorage(MatrixRegister(mainCamera.mViewMatrix) * XMMatrixRotationY(-delta * rotationSpeed));

		XMFLOAT4 hey = VectorStorage(pos);

		mainCamera.mViewMatrix._41 = hey.x;
		mainCamera.mViewMatrix._42 = hey.y;
		mainCamera.mViewMatrix._43 = hey.z;


		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LSHIFT))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixRotationZ(delta * rotationSpeed) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_RSHIFT))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixRotationZ(-delta * rotationSpeed) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	//Zoom level
	if(GetAsyncKeyState('Z') )
	{
		if(mainCamera.FOV > 20)
		{
			mainCamera.FOV -= (delta * 30);
			UpdateProjection();
		}
	}

	if(GetAsyncKeyState('X'))
	{
		if(mainCamera.FOV < 120)
		{
			mainCamera.FOV += (delta * 30);
			UpdateProjection();
		}

	}

	//Near Plane
	if(GetAsyncKeyState(VK_NUMPAD4))
	{
		if(mainCamera.nearPlane < 20)
		{
			mainCamera.nearPlane += delta * 15;
			UpdateProjection();
		}

	}

	if(GetAsyncKeyState(VK_NUMPAD1))
	{
		if(mainCamera.nearPlane > 0.1f)
		{
			mainCamera.nearPlane -= delta * 15;
			UpdateProjection();
		}
	}

	//Far Plane
	if(GetAsyncKeyState(VK_NUMPAD6))
	{
		if(mainCamera.farPlane < 500)
		{
			mainCamera.farPlane += delta * 15;
			UpdateProjection();
		}
	}

	if(GetAsyncKeyState(VK_NUMPAD3))
	{
		if(mainCamera.farPlane > 25)
		{
			mainCamera.farPlane -= delta * 15;
			UpdateProjection();
		}

	}

	//Reset camera 
	if(GetAsyncKeyState('R'))
	{
		float aspectRatio;
		mySurface->GetAspectRatio(aspectRatio);

		mainCamera.InitializeCamera(aspectRatio);

		updateCameraBuffer = true;
	}

	//Mouse input?
	POINT mousePos;
	
	if(GetCursorPos(&mousePos))
	{	
	}
}

void LetsDrawSomeStuff::UpdateProjection()
{
	float aspectRatio;
	mySurface->GetAspectRatio(aspectRatio);
	mainCamera.mProjMatrix = MatrixStorage(XMMatrixPerspectiveFovLH(XMConvertToRadians(mainCamera.FOV), aspectRatio, mainCamera.nearPlane, mainCamera.farPlane));
	updateCameraBuffer = true;
}

void LetsDrawSomeStuff::InitializeGruntData()
{
	gruntSpawnpoints[0] = XMFLOAT4(18.8939f, -23.3933f, -13.5918f, 1);
	gruntSpawnpoints[1] = XMFLOAT4(-24.2221, 5.42948f, -22.0524f, 1);
	gruntSpawnpoints[2] = XMFLOAT4(-34.9067f, -23.3924f, -30.3552f, 1);
	gruntSpawnpoints[3] = XMFLOAT4(20.5216f, -23.7873f, 35.1893f, 1);
	gruntSpawnpoints[4] = XMFLOAT4(19.6623f, 3.42186f, 10.0884f, 1);

	gruntColors[0] = XMFLOAT4(0.1568f, 0.4470f, 0.1843f,	1); //green
	gruntColors[1] = XMFLOAT4(0.0941f, 0.0941f, 0.3372f, 1);	//blue
	gruntColors[2] = XMFLOAT4(0.5098f, 0.0392f, 0.0392f, 1);	//Red
	gruntColors[3] = XMFLOAT4(0.8078f, 0.3372f, 0, 1);			//Orage
	gruntColors[4] = XMFLOAT4(1, 1, 1, 1);						//White


}