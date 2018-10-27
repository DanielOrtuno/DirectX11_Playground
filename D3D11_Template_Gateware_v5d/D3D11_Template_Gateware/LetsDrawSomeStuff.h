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
			triangle.MakePyramid(myDevice);

			XMFLOAT4X4 triangleWorldMatrix = MatrixRegisterToStorage(XMMatrixIdentity() * XMMatrixTranslation(0,0,1));

			triangle.SetWorldMatrix(triangleWorldMatrix);

			
			hammer.LoadMeshFromHeader(myDevice, ThorHammer_data, 884, ThorHammer_indicies , 1788);

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
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

			if(updateCameraBuffer)
			{
				CameraConstantBuffer ccb = {  };

				ccb.viewMatrix = MatrixRegisterToStorage(XMMatrixInverse(nullptr, MatrixStorageToRegister(mainCamera.mViewMatrix)));
				ccb.projMatrix = mainCamera.mProjMatrix;

				D3D11_MAPPED_SUBRESOURCE data = { 0 };
				myContext->Map(cameraConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
				memcpy_s(data.pData, sizeof(ccb), &ccb, sizeof(ccb));
				myContext->Unmap(cameraConstBuff, 0);

				myContext->VSSetConstantBuffers(0, 1, &cameraConstBuff.p);

				updateCameraBuffer = false;
			}

			MeshConstantBuffer mcb = {  };

			mcb.worldMatrix = triangle.GetWorldMatrix();

			D3D11_MAPPED_SUBRESOURCE data = { 0 };
			myContext->Map(meshConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(mcb), &mcb, sizeof(mcb));
			myContext->Unmap(meshConstBuff, 0);

			myContext->VSSetConstantBuffers(1, 1, &meshConstBuff.p);


			triangle.RenderMesh(myContext, vertexShader.p, pixelShader.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			hammer.RenderMesh(myContext, vertexShader.p, pixelShader.p, inputLayout.p, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				
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
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixTranslation(0.0f, delta, 0.0f) * MatrixStorageToRegister(mainCamera.mViewMatrix));
		updateCameraBuffer = true;
	}

	if(GetAsyncKeyState(VK_LCONTROL))
	{
		mainCamera.mViewMatrix = MatrixRegisterToStorage(XMMatrixTranslation(0.0f, -delta, 0.0f) * MatrixStorageToRegister(mainCamera.mViewMatrix));
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