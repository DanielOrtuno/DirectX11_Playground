#include "Mesh.h"

Mesh::Mesh()
{
	m_pVertexBuffer.p = nullptr;
	m_pIndexBuffer.p = nullptr;
	m_pVertices = nullptr;
	m_pIndices = nullptr;

	mNumVertices = 0;
	mNumIndices = 0;
}


void Mesh::MakePyramid(ID3D11Device* device)
{
	// Load data into arrays
	mNumVertices = 4;
	m_pVertices = new VERTEX[mNumVertices];

	m_pVertices[0] = { XMFLOAT4(0.0f, 0.5f, 0.25f, 1.0f),	XMFLOAT4(1,0,0,1) };
	m_pVertices[1] = { XMFLOAT4(0.25f, -0.5f, 0.0f, 1.0f),	XMFLOAT4(1,0,0,1) };
	m_pVertices[2] = { XMFLOAT4(-0.25f, -0.5f, 0.0f, 1.0f),	XMFLOAT4(1,0,0,1) };
	m_pVertices[3] = { XMFLOAT4(0.25f, -0.5f, 0.5f, 1.0f),	XMFLOAT4(1,0,0,1) };



	mNumIndices = 12;
	m_pIndices = new int[mNumIndices];

	m_pIndices[0] = 0;	//Front face
	m_pIndices[1] = 1;
	m_pIndices[2] = 2;

	m_pIndices[3] = 0;	//Left face
	m_pIndices[4] = 2;
	m_pIndices[5] = 3;

	m_pIndices[6] = 0;	//Right face
	m_pIndices[7] = 3;
	m_pIndices[8] = 1;

	m_pIndices[9] =	 1;	//Bottom face
	m_pIndices[10] = 2;
	m_pIndices[11] = 3;

	
	//Create buffers
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(VERTEX) * mNumVertices;
	bufferDesc.CPUAccessFlags = 0;
	
	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = m_pVertices;

	device->CreateBuffer(&bufferDesc, &data, &m_pVertexBuffer.p);

	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(int) * mNumIndices;
	
	data.pSysMem = m_pIndices;

	device->CreateBuffer(&bufferDesc, &data, &m_pIndexBuffer.p);


}

int Mesh::InitializeAsCube(ID3D11Device* device)
{
	return 0;
}

int Mesh::LoadMeshFromFile(ID3D11Device* device)
{
	return 0;
}

int Mesh::RenderMesh(ID3D11DeviceContext* context, ID3D11VertexShader* VS, ID3D11PixelShader* PS, ID3D11InputLayout* inputLayout, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	context->IASetPrimitiveTopology(topology);

	context->IASetInputLayout(inputLayout);

	const UINT stride = sizeof(VERTEX);
	const UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer.p, &stride, &offset);
	context->IASetIndexBuffer(m_pIndexBuffer.p, DXGI_FORMAT_R32_UINT, 0);

	context->VSSetShader(VS, nullptr, 0);
	context->PSSetShader(PS, nullptr, 0);

	context->DrawIndexed(mNumIndices, 0, 0);

	//context->Draw(3, 0);

	return 0;
}


void Mesh::SetWorldMatrix(XMFLOAT4X4 SetWorldMatrix)
{
	mWorldMatrix = SetWorldMatrix;
}

XMFLOAT4X4 Mesh::GetWorldMatrix()
{
	return mWorldMatrix;
}

Mesh::~Mesh()
{
	delete[] m_pVertices;
	delete[] m_pIndices;
}
