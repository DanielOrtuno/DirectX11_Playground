#pragma once

#include "defines.h"
#include <d3dcompiler.h>
#include <iostream>
#include <vector>
#include "DDSTextureLoader.h"

using namespace DirectX;


struct VERTEX
{
	XMFLOAT4 pos;
	XMFLOAT3 uv;
	XMFLOAT3 normal;
};


class Mesh
{
private:
	
	XMFLOAT4X4							mWorldMatrix;
	CComPtr<ID3D11Buffer>				m_pVertexBuffer;
	CComPtr<ID3D11Buffer>				m_pIndexBuffer;
	CComPtr<ID3D11Texture2D>			m_pDiffuseMap;
	CComPtr<ID3D11ShaderResourceView>	m_pSRV;

	VERTEX*								m_pVertices;
	int*								m_pIndices;

	int		/*This is an integer */		mNumVertices;
	int									mNumIndices;

public:
	CComPtr<ID3D11SamplerState>			m_pSamplerState;

	Mesh();

	void MakePyramid(ID3D11Device* device);

	int InitializeAsCube(ID3D11Device* device);

	void InitializeAs3DGrid(ID3D11Device* device);

	int LoadMeshFromFile(ID3D11Device* device, const wchar_t filename[]);

	void LoadMeshFromHeader(ID3D11Device* device, const _OBJ_VERT_* vertArray, int vertexCount, const unsigned int* indexArray, int indexCount);

	void LoadTexture(ID3D11Device* device, const wchar_t filename[]);

	int RenderMesh(ID3D11DeviceContext* context, ID3D11VertexShader* VS, ID3D11PixelShader* PS, ID3D11InputLayout* inputLayout, D3D11_PRIMITIVE_TOPOLOGY topology);

	XMFLOAT4X4 GetWorldMatrix();

	void SetWorldMatrix(XMFLOAT4X4 newMatrix);


	void TestGrid(ID3D11DeviceContext* context, ID3D11VertexShader* VS, ID3D11PixelShader* PS, ID3D11InputLayout* inputLayout, D3D11_PRIMITIVE_TOPOLOGY topology)
	{

		context->IASetPrimitiveTopology(topology);

		context->IASetInputLayout(inputLayout);

		const UINT stride = sizeof(VERTEX);
		const UINT offset = 0;

		context->IASetVertexBuffers(0, 1, &m_pVertexBuffer.p, &stride, &offset);

		context->VSSetShader(VS, nullptr, 0);
		context->PSSetShader(PS, nullptr, 0);

		context->Draw(mNumVertices, 0);
	}

	~Mesh();
};

