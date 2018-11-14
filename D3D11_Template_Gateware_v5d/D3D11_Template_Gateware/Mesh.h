#pragma once

#include "defines.h"
#include <d3dcompiler.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "DDSTextureLoader.h"
#include <algorithm>
#include <d3d11.h>

using namespace DirectX;


struct VERTEX
{
	XMFLOAT4 pos;
	XMFLOAT3 uv;
	XMFLOAT3 normal;

	bool operator==(const VERTEX& v) const
	{
		if(this->pos.x != v.pos.x || this->pos.y != v.pos.y || this->pos.z != v.pos.z || this->pos.w != v.pos.w)
			return false;

		if(this->uv.x != v.uv.x || this->uv.y != v.uv.y || this->uv.z != v.uv.z)
			return false;

		if(this->normal.x != v.normal.x || this->normal.y != v.normal.y || this->normal.z != v.normal.z)
			return false;

		return true;
	}
};

struct hashFct_VERTEX
{
	size_t operator() (const VERTEX& v) const
	{
		size_t pos = std::hash<float>()( v.pos.x ) ^ std::hash<float>()( v.pos.y ) ^ std::hash<float>()( v.pos.z ) ^ std::hash<float>()( v.pos.w );
		size_t uv = std::hash<float>()( v.uv.x ) ^ std::hash<float>()( v.uv.y ) ^ std::hash<float>()( v.uv.z );
		size_t norm = std::hash<float>()( v.normal.x ) ^ std::hash<float>()( v.normal.y ) ^ std::hash<float>()( v.normal.z );

		return pos ^ uv ^ norm;
	}
};

struct InstanceType
{
	XMFLOAT4 posAndRot; //w component is the y rotation
	XMFLOAT4 tintColor;
};

class Mesh
{
private:
	
	CComPtr<ID3D11Buffer>				m_pVertexBuffer;
	CComPtr<ID3D11Buffer>				m_pIndexBuffer;
	CComPtr<ID3D11Buffer>				m_pInstanceBuffer;
	CComPtr<ID3D11Texture2D>			m_pDiffuseMap;
	CComPtr<ID3D11ShaderResourceView>	m_pSRV;


	VERTEX*								m_pVertices;
	InstanceType*						m_pInstanceData;
	int*								m_pIndices;

	int		/*This is an integer */		m_NumVertices;
	int									m_NumIndices;
	int									m_NumInstances;


	void CreateBuffers(ID3D11Device* device);

public:
	CComPtr<ID3D11RenderTargetView>		m_pRenderTargetView;

	XMFLOAT4 baseColor;
	XMFLOAT4X4							mWorldMatrix;

	
	CComPtr<ID3D11SamplerState>			m_pSamplerState;

	Mesh();

	void MakePyramid(ID3D11Device* device);

	int CreateSkybox(ID3D11Device* device, ID3D11DeviceContext* context, const char boxpath[],const wchar_t texturePath[]);


	void InitializeAs3DGrid(ID3D11Device* device);

	int LoadMeshFromFile(ID3D11Device* device, const char filename[], bool flipV);

	void LoadMeshFromHeader(ID3D11Device* device, const _OBJ_VERT_* vertArray, int vertexCount, const unsigned int* indexArray, int indexCount);

	void LoadTexture(ID3D11Device* device, const wchar_t filename[]);

	int RenderMesh(ID3D11DeviceContext* context, ID3D11VertexShader* VS, ID3D11PixelShader* PS, ID3D11InputLayout* inputLayout, D3D11_PRIMITIVE_TOPOLOGY topology);

	int RenderInstancesOfMesh(ID3D11DeviceContext* context, ID3D11VertexShader* VS, ID3D11PixelShader* PS, ID3D11InputLayout* inputLayout, D3D11_PRIMITIVE_TOPOLOGY topology);

	XMFLOAT4X4 GetWorldMatrix();

	void SetWorldMatrix(XMFLOAT4X4 newMatrix);

	void SetInstancingData(int _NumInstances, InstanceType _instanceData[]);

	void TestGrid(ID3D11DeviceContext* context, ID3D11VertexShader* VS, ID3D11PixelShader* PS, ID3D11InputLayout* inputLayout, D3D11_PRIMITIVE_TOPOLOGY topology)
	{

		context->IASetPrimitiveTopology(topology);

		context->IASetInputLayout(inputLayout);

		const UINT stride = sizeof(VERTEX);
		const UINT offset = 0;

		context->IASetVertexBuffers(0, 1, &m_pVertexBuffer.p, &stride, &offset);

		context->VSSetShader(VS, nullptr, 0);
		context->PSSetShader(PS, nullptr, 0);

		context->Draw(m_NumVertices, 0);
	}

	void BreakUV(float mul)
	{
		for(int i = 0; i < m_NumVertices; i++)
		{
			m_pVertices[i].uv.x *= mul;
			m_pVertices[i].uv.y *= mul;
		}

	}

	void CreateTextureManually(ID3D11Device* myDevice)
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = 1008;
		textureDesc.Height = 729;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// Create the texture
		myDevice->CreateTexture2D(&textureDesc, nullptr, &m_pDiffuseMap.p);

		rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		myDevice->CreateRenderTargetView(m_pDiffuseMap, &rtvDesc, &m_pRenderTargetView.p);

		srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		myDevice->CreateShaderResourceView(m_pDiffuseMap, &srvDesc, &m_pSRV.p);

	}
	~Mesh();
};

