#pragma once

#include "defines.h"
#include <d3dcompiler.h>
#include <iostream>
#include <fbxsdk.h>

using namespace DirectX;


struct VERTEX
{
	XMFLOAT4 pos;
	XMFLOAT4 color;
};


class Mesh
{
private:
	
	XMFLOAT4X4 mWorldMatrix;
	CComPtr<ID3D11Buffer> m_pVertexBuffer;
	CComPtr<ID3D11Buffer> m_pIndexBuffer;

	VERTEX* m_pVertices;
	int*	m_pIndices;

	int mNumVertices;
	int mNumIndices;

public:
	Mesh();


	void MakePyramid(ID3D11Device* device);

	int LoadMeshFromFile(ID3D11Device* device);

	int InitializeAsCube(ID3D11Device* device);

	int RenderMesh(ID3D11DeviceContext* context, ID3D11VertexShader* VS, ID3D11PixelShader* PS, ID3D11InputLayout* inputLayout, D3D11_PRIMITIVE_TOPOLOGY topology);

	XMFLOAT4X4 GetWorldMatrix();

	void SetWorldMatrix(XMFLOAT4X4 newMatrix);

	~Mesh();
};

