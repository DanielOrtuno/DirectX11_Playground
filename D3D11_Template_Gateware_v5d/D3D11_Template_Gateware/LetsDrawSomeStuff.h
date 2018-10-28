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

// Simple Container class to make life easier/cleaner
using namespace DirectX;

struct MeshConstantBuffer
{
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4 color;
	int enableTexture;
	int x;
	int y;
	int z;
};

struct LightConstantBuffer
{
	XMFLOAT4 lightDirection[2];
	XMFLOAT4 lightColor[2];
};

struct CameraConstantBuffer
{
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;
};


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
	CComPtr<ID3D11InputLayout> inputLayout = nullptr;
	CComPtr<ID3D11Buffer> cameraConstBuff = nullptr;
	CComPtr<ID3D11Buffer> meshConstBuff = nullptr;
	CComPtr<ID3D11Buffer> lightConstBuff = nullptr;

	Mesh triangle;
	Mesh hammer;

	Camera mainCamera;

	bool updateCameraBuffer;

public:

	XTime timer;
	// Init
	LetsDrawSomeStuff(GW::SYSTEM::GWindow* attatchPoint);
	// Shutdown
	~LetsDrawSomeStuff();
	// Draw
	void Render();

	void ManageUserInput();
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

			// Grab handles to all DX11 base interfaces
			mySurface->GetDevice((void**)&myDevice);
			mySurface->GetSwapchain((void**)&mySwapChain);
			mySurface->GetContext((void**)&myContext);

			// TODO: Create new DirectX stuff here! (Buffers, Shaders, Layouts, Views, Textures, etc...)

			//Models
			triangle.InitializeAs3DGrid(myDevice);

			XMFLOAT4X4 triangleWorldMatrix = MatrixRegisterToStorage(XMMatrixIdentity() * XMMatrixTranslation(0,0,1));

			triangle.SetWorldMatrix(triangleWorldMatrix);

			
			hammer.LoadMeshFromHeader(myDevice, ThorHammer_data, 884, ThorHammer_indicies , 1788);

			hammer.LoadTexture(myDevice, L"../D3D11_Template_Gateware/Models/HammerTexture.dds");

			D3D11_SAMPLER_DESC desc = { };
			ZeroMemory(&desc, sizeof(desc));

			desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MinLOD = 0;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			
			myDevice->CreateSamplerState(&desc, &hammer.m_pSamplerState.p);

			triangle.SetWorldMatrix(MatrixRegisterToStorage(XMMatrixIdentity()));
	
			//Camera

			float aspectRatio;
			mySurface->GetAspectRatio(aspectRatio);

			mainCamera.InitializeCamera(aspectRatio);

			//Loading Shaders

			myDevice->CreateVertexShader(BasicVertexShader, sizeof(BasicVertexShader), nullptr, &vertexShader.p);
			myDevice->CreatePixelShader(BasicPixelShader, sizeof(BasicPixelShader), nullptr, &pixelShader.p);

			D3D11_INPUT_ELEMENT_DESC layout[]
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXTURECOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			myDevice->CreateInputLayout(layout, 3, BasicVertexShader, sizeof(BasicVertexShader), &inputLayout.p);


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

			// Clear the screen to dark green
			const float d_green[] = { 0, 0, 0, 1 };
			myContext->ClearRenderTargetView(myRenderTargetView, d_green);
			
			// TODO: Set your shaders, Update & Set your constant buffers, Attatch your vertex & index buffers, Set your InputLayout & Topology & Draw!

			D3D11_MAPPED_SUBRESOURCE data = { 0 };

			//Update Camera Buffer
			if(updateCameraBuffer)
			{
				CameraConstantBuffer ccb = {  };

				ccb.viewMatrix = MatrixRegisterToStorage(XMMatrixInverse(nullptr, MatrixStorageToRegister(mainCamera.mViewMatrix)));
				ccb.projMatrix = mainCamera.mProjMatrix;

				
				myContext->Map(cameraConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(ccb), &ccb, sizeof(ccb));
				myContext->Unmap(cameraConstBuff, 0);

				myContext->VSSetConstantBuffers(0, 1, &cameraConstBuff.p);

				updateCameraBuffer = false;
			}

			//Update light buffer

			LightConstantBuffer lcb = {  };
			lcb.lightDirection[0] = XMFLOAT4(0.577f, -0.6f, 0.577f, 1.0f);
			lcb.lightDirection[1] = XMFLOAT4(-0.577f, 0.6f, -0.577f, 1.0f);
			lcb.lightColor[0] = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.1f);
			lcb.lightColor[1] = XMFLOAT4(0.2f, 0.1f, 0.8f, 0.1f);

			data = { 0 };
			myContext->Map(lightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(lcb), &lcb, sizeof(lcb));
			myContext->Unmap(lightConstBuff, 0);

			myContext->VSSetConstantBuffers(2, 1, &lightConstBuff.p);

			//Update Mesh buffer for Mjölnir 
			MeshConstantBuffer mcb = {  };

			mcb.worldMatrix = triangle.GetWorldMatrix();
			mcb.color = XMFLOAT4{ 0,1,0,0 };
			mcb.enableTexture = 1;

			data = { 0 };
			myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
			myContext->Unmap(meshConstBuff, 0);

			myContext->VSSetConstantBuffers(1, 1, &meshConstBuff.p);

			hammer.RenderMesh(myContext, vertexShader.p, pixelShader.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				

			//Update Mesh buffer for Grid
			data = { 0 };
			mcb.enableTexture = 0;
			myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
			myContext->Unmap(meshConstBuff, 0);

			myContext->VSSetConstantBuffers(1, 1, &meshConstBuff.p);

			triangle.TestGrid(myContext, vertexShader.p, pixelShader.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
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
	
	//Camera Movement
	if(GetAsyncKeyState('W'))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage( XMMatrixTranslation(0.0f, 0.0f, delta) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState('A'))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixTranslation(-delta, 0.0f, 0.0f) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState('S'))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixTranslation(0.0f, 0.0f, -delta) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState('D'))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixTranslation(delta, 0.0f, 0.0f) *  MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_SPACE))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixIdentity() * XMMatrixTranslation(0.0f, delta, 0.0f) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LCONTROL))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixIdentity() * XMMatrixTranslation(0.0f, -delta, 0.0f) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	//Camera Rotation

	if(GetAsyncKeyState(VK_UP))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixRotationX(-delta) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_RIGHT))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage( MatrixStorageToRegister(mainCamera.mViewMatrix) * XMMatrixRotationY(delta));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_DOWN))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixRotationX(delta) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LEFT))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(MatrixStorageToRegister(mainCamera.mViewMatrix) * XMMatrixRotationY(-delta));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LSHIFT))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixRotationZ(delta) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_RSHIFT))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixRotationZ(-delta) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}


}