// This file can be used to contain very basic DX11 Loading, Drawing & Clean Up. (Start Here, or Make your own set of classes)
#pragma once
// Include our DX11 middle ware
#include "Gateware Redistribution R5d/Interface/G_Graphics/GDirectX11Surface.h"

// Include DirectX11 for interface access

#include "Mesh.h" //Mesh.h has all the includes for DirectX
#include "Camera.h"
#include "XTime.h"
#include "BasicVertexShader.csh"
#include "BasicPixelShader.csh"
#include "GridVertexShader.csh"

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
	CComPtr<ID3D11PixelShader> pixelShader = nullptr;
	CComPtr<ID3D11VertexShader> vertexShader = nullptr;
	CComPtr<ID3D11VertexShader> gridShader = nullptr;
	CComPtr<ID3D11InputLayout> inputLayout = nullptr;

	CComPtr<ID3D11Buffer> cameraConstBuff = nullptr;
	CComPtr<ID3D11Buffer> meshConstBuff = nullptr;
	CComPtr<ID3D11Buffer> lightConstBuff = nullptr;
	CComPtr<ID3D11Buffer> pixelConstBuff = nullptr;
	
	Mesh grid;
	Mesh hammer;
	Mesh livingroom;

	Camera mainCamera;

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

	void UpdateProjection(GW::SYSTEM::GWindow* attatchPoint);
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

			#pragma region Camera

			float aspectRatio;
			mySurface->GetAspectRatio(aspectRatio);

			mainCamera.InitializeCamera(aspectRatio);

			#pragma endregion


			#pragma region Meshes

			//Grid
			grid.InitializeAs3DGrid(myDevice);
			XMFLOAT4X4 gridWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(0,4.5f,0));
			grid.SetWorldMatrix(gridWorldMatrix);


			//Mj�lnir
			XMFLOAT4X4 hammerWorldMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(0,1.5f,0));
			hammer.SetWorldMatrix(hammerWorldMatrix);
			hammer.LoadMeshFromHeader(myDevice, ThorHammer_data, 884, ThorHammer_indicies , 1788);
			hammer.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/HammerTexture.dds");

			//Livingroom
			XMFLOAT4X4 livingroomWorldMatrix = MatrixStorage(XMMatrixIdentity());
			livingroom.SetWorldMatrix(livingroomWorldMatrix);
			livingroom.LoadMeshFromHeader(myDevice, LivingRoom_data, 30224, LivingRoom_indicies, 31140);

			#pragma endregion 
			

			#pragma region Sampler

			D3D11_SAMPLER_DESC desc = { };
			ZeroMemory(&desc, sizeof(desc));

			desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MinLOD = 0;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			
			myDevice->CreateSamplerState(&desc, &hammer.m_pSamplerState.p);

			#pragma endregion


			#pragma region Shaders

			myDevice->CreateVertexShader(BasicVertexShader, sizeof(BasicVertexShader), nullptr, &vertexShader.p);
			myDevice->CreatePixelShader(BasicPixelShader, sizeof(BasicPixelShader), nullptr, &pixelShader.p);
			myDevice->CreateVertexShader(GridVertexShader, sizeof(GridVertexShader), nullptr, &gridShader.p);

			#pragma endregion


			#pragma region Input Layout

			D3D11_INPUT_ELEMENT_DESC layout[]
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXTURECOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			myDevice->CreateInputLayout(layout, 3, BasicVertexShader, sizeof(BasicVertexShader), &inputLayout.p);

			#pragma endregion


			#pragma region  Constant Buffers

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

			D3D11_MAPPED_SUBRESOURCE data = { 0 };

			//Update Camera Buffer
			if(updateCameraBuffer)
			{
				CameraConstantBuffer ccb = {  };

				ccb.viewMatrix = MatrixStorage(XMMatrixInverse(nullptr, MatrixRegister(mainCamera.mViewMatrix)));
				ccb.projMatrix = mainCamera.mProjMatrix;

				
				myContext->Map(cameraConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(ccb), &ccb, sizeof(ccb));
				myContext->Unmap(cameraConstBuff, 0);

				myContext->VSSetConstantBuffers(0, 1, &cameraConstBuff.p);

				updateCameraBuffer = false;
			}

			//Update lights

			LightConstantBuffer lcb = {  };
			lcb.lightDirection[0] = XMFLOAT4(0.577f, -0.6f, 0.577f, 1.0f);
			lcb.lightDirection[1] = XMFLOAT4(0, 0, 1, 1.0f);
			lcb.lightColor[0] = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.1f);
			lcb.pointlightPos = XMFLOAT4(-7, 2.2f, 2, 1);
			lcb.pointlightColor = XMFLOAT4(1, 0, 0, 1);


			if(turnOnSecondLight)
				lcb.lightColor[1] = XMFLOAT4(0.2f, 0.1f, 0.8f, 0.1f);
			else
				lcb.lightColor[1] = XMFLOAT4(0,0,0,0);

			lcb.lightDirection[1] = VectorStorage(XMVector3Rotate(VectorRegister(lcb.lightDirection[1]), XMQuaternionRotationMatrix(XMMatrixRotationY((float)timer.TotalTime() * 2))));

			data = { 0 };
			myContext->Map(lightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(lcb), &lcb, sizeof(lcb));
			myContext->Unmap(lightConstBuff, 0);
			
			myContext->PSSetConstantBuffers(0, 1, &lightConstBuff.p);

			//Mj�lnir 
			MeshConstantBuffer mcb = {  };
			mcb.worldMatrix = hammer.GetWorldMatrix();
			mcb.enableTexture = 1;
			mcb.time = timer.TotalTime();
			mcb.dir = gridDirection;

			gridTimer += timer.Delta();
			
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

			hammer.RenderMesh(myContext, vertexShader.p, pixelShader.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				
			XMVECTOR lightPosition = VectorRegister(lcb.lightDirection[1]);
			lightPosition *= VectorRegister(XMFLOAT4(3,1,3,1));

			XMFLOAT4X4 lightMatrix = (MatrixStorage(XMMatrixScaling(.2f, .2f, .2f) * XMMatrixTranslationFromVector(-lightPosition * 5) * XMMatrixTranslation(0,5,0)));

			//Second Mjolnir 
			data = { 0 };
			mcb.enableTexture = 0;
			mcb.color = XMFLOAT4( 0.2f, 0.1f, 0.8f, 0.1f );
			mcb.worldMatrix = lightMatrix;
			myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
			myContext->Unmap(meshConstBuff, 0);

			myContext->VSSetConstantBuffers(1, 1, &meshConstBuff.p);

			hammer.RenderMesh(myContext, vertexShader.p, pixelShader.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Third Mjolnir
			lightMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixScaling(.2f, .2f, .2f));

			lightMatrix._41 = lcb.pointlightPos.x;
			lightMatrix._42 = lcb.pointlightPos.y;
			lightMatrix._43 = lcb.pointlightPos.z;

			data = { 0 };
			mcb.enableTexture = 0;
			mcb.color = XMFLOAT4(0.5f, 0, 0, 1);
			mcb.worldMatrix = lightMatrix;
			myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
			myContext->Unmap(meshConstBuff, 0);

			myContext->VSSetConstantBuffers(1, 1, &meshConstBuff.p);

			hammer.RenderMesh(myContext, vertexShader.p, pixelShader.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//Grid
			mcb.color = XMFLOAT4(0.0f,1.0f,0.0f, 0.5f);
			mcb.worldMatrix = grid.GetWorldMatrix();
			myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
			myContext->Unmap(meshConstBuff, 0);

			grid.TestGrid(myContext, gridShader.p, pixelShader.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			//Livingroom
			mcb.color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			mcb.worldMatrix = livingroom.GetWorldMatrix();
			myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
			myContext->Unmap(meshConstBuff, 0);
			

			livingroom.RenderMesh(myContext, vertexShader.p, pixelShader.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
	float delta = (float)timer.Delta() * 1.5f;
	
	if(GetAsyncKeyState('I') & 0x1)
	{
		turnOnSecondLight = !turnOnSecondLight;
	}

	//Camera Movement
	if(GetAsyncKeyState('W'))
	{
		mainCamera.mViewMatrix = MatrixStorage( XMMatrixTranslation(0.0f, 0.0f, delta * 2) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState('A'))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixTranslation(-delta * 2, 0.0f, 0.0f) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState('S'))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixTranslation(0.0f, 0.0f, -delta * 2) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState('D'))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixTranslation(delta * 2, 0.0f, 0.0f) *  MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_SPACE))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(0.0f, delta * 2, 0.0f) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LCONTROL))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixIdentity() * XMMatrixTranslation(0.0f, -delta * 2, 0.0f) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	//Camera Rotation

	if(GetAsyncKeyState(VK_UP))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixRotationX(-delta) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_RIGHT))
	{
		XMVECTOR pos;
		XMVECTOR rot;
		XMVECTOR scale;

		XMMatrixDecompose(&scale, &rot, &pos, MatrixRegister(mainCamera.mViewMatrix));
		
		mainCamera.mViewMatrix._41, mainCamera.mViewMatrix._42, mainCamera.mViewMatrix._43 = 0;

		mainCamera.mViewMatrix = MatrixStorage( MatrixRegister(mainCamera.mViewMatrix) * XMMatrixRotationY(delta) );

		XMFLOAT4 hey = VectorStorage(pos);

		mainCamera.mViewMatrix._41 = hey.x;
		mainCamera.mViewMatrix._42 = hey.y;
		mainCamera.mViewMatrix._43 = hey.z;

		updateCameraBuffer = true;								

	}

	if(GetAsyncKeyState(VK_DOWN))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixRotationX(delta) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LEFT))
	{
		XMVECTOR pos;
		XMVECTOR rot;
		XMVECTOR scale;

		XMMatrixDecompose(&scale, &rot, &pos, MatrixRegister(mainCamera.mViewMatrix));

		mainCamera.mViewMatrix._41, mainCamera.mViewMatrix._42, mainCamera.mViewMatrix._43 = 0;

		mainCamera.mViewMatrix = MatrixStorage(MatrixRegister(mainCamera.mViewMatrix) * XMMatrixRotationY(-delta));

		XMFLOAT4 hey = VectorStorage(pos);

		mainCamera.mViewMatrix._41 = hey.x;
		mainCamera.mViewMatrix._42 = hey.y;
		mainCamera.mViewMatrix._43 = hey.z;


		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LSHIFT))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixRotationZ(delta) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_RSHIFT))
	{
		mainCamera.mViewMatrix = MatrixStorage(XMMatrixRotationZ(-delta) * MatrixRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}


}

void LetsDrawSomeStuff::UpdateProjection(GW::SYSTEM::GWindow* attatchPoint)
{
	float aspectRatio;
	mySurface->GetAspectRatio(aspectRatio);
	mainCamera.mProjMatrix = MatrixStorage(XMMatrixPerspectiveFovLH(65, aspectRatio, .1f, 10000));
	updateCameraBuffer = true;
}