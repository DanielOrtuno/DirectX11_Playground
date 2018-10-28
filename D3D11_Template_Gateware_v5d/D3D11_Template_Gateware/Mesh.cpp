#include "Mesh.h"

Mesh::Mesh()
{
	m_pVertexBuffer.p = nullptr;
	m_pIndexBuffer.p = nullptr;
	m_pVertices = nullptr;
	m_pIndices = nullptr;
	m_pDiffuseMap.p = nullptr;
	m_pSamplerState.p = nullptr;

	mNumVertices = 0;
	mNumIndices = 0;
}



void Mesh::MakePyramid(ID3D11Device* device)
{
	// Load data into arrays
	mNumVertices = 4;
	m_pVertices = new VERTEX[mNumVertices];

	m_pVertices[0] = { XMFLOAT4(0.0f, 0.5f, 0.25f, 1.0f),	XMFLOAT3(1,0,0) };
	m_pVertices[1] = { XMFLOAT4(0.25f, -0.5f, 0.0f, 1.0f),	XMFLOAT3(1,0,0) };
	m_pVertices[2] = { XMFLOAT4(-0.25f, -0.5f, 0.0f, 1.0f),	XMFLOAT3(1,0,0) };
	m_pVertices[3] = { XMFLOAT4(0.25f, -0.5f, 0.5f, 1.0f),	XMFLOAT3(1,0,0) };


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

void Mesh::InitializeAs3DGrid(ID3D11Device* device)
{
	std::vector<VERTEX> list;

	float posY = -0.5f;



	//for(int i = 0; i < 11; i++)
	//{
		float pos = -0.5f;
		for(int i = 0; i < 22; i += 2)
		{
			VERTEX newVertex;

			//Col
			newVertex = { XMFLOAT4(pos, posY, -.5f, 1.0f) };
			list.push_back(newVertex);

			newVertex = { XMFLOAT4(pos, posY, .5f, 1.0f) };
			list.push_back(newVertex);


			//Row
			newVertex = { XMFLOAT4(-.5f, posY, pos, 1.0f) };
			list.push_back(newVertex);

			newVertex = { XMFLOAT4( .5f, posY, pos, 1.0f ) };
			list.push_back(newVertex);


			//North Korea
			newVertex = { XMFLOAT4(-.5f, posY, pos, 1.0f) };
			list.push_back(newVertex);

			newVertex = { XMFLOAT4(.5f, posY, pos, 1.0f) };
			list.push_back(newVertex);

			pos += .1f;d
		}

	////	posY += .1f;
	////}

	mNumVertices = list.size();
	m_pVertices = new VERTEX[mNumVertices];

	for(int i = 0; i < mNumVertices; i++)
	{
		m_pVertices[i] = list[i];
	}

	//Create buffers
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(VERTEX) * mNumVertices;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = m_pVertices;

	device->CreateBuffer(&bufferDesc, &data, &m_pVertexBuffer.p);

}

int Mesh::LoadMeshFromFile(ID3D11Device* device, const wchar_t filename[])
{
	FILE* file = fopen("..//D3D11_Template_Gateware//TestFile.obj", "r");

	if(file == NULL)
		return 1;
	
	std::vector<XMFLOAT4> posiciones;
	std::vector<XMFLOAT2> uv;
	std::vector<XMFLOAT3> normales;
	std::vector<int>	  vertexIndices;
	std::vector<int>	  uvIndices;
	std::vector<int>	  normalIndices;


	while(true)
	{
		char buffer[100];
		int result = fscanf(file, "%s", buffer);

		if(result == NULL)
			break;

		if(strcmp(buffer, "v"))
		{
			XMFLOAT4 newData;
			fscanf(file, "%f %f %f\n", &newData.x, &newData.y, &newData.z);

			newData.w = 1.0f;
			posiciones.push_back(newData);
		}
		else if(strcmp(buffer, "vt"))
		{
			XMFLOAT2 newData;
			fscanf(file, "%f %f\n", &newData.x, &newData.y);

			uv.push_back(newData);
		}
		else if(strcmp(buffer, "vn"))
		{
			XMFLOAT3 newData;
			fscanf(file, "%f %f %f\n", &newData.x, &newData.y, &newData.z);

			normales.push_back(newData);
		}
		else if(strcmp(buffer, "f"))
		{
			int x[3], y[3], z[3];

			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &x[0], &y[0], &z[0], &x[1], &y[1], &z[1], &x[2], &y[2], &z[2]);
			if(matches != 9) 
				return 2;
			
			vertexIndices.push_back(x[0]);
			vertexIndices.push_back(x[1]);
			vertexIndices.push_back(x[2]);

			uvIndices.push_back(y[0]);
			uvIndices.push_back(y[1]);
			uvIndices.push_back(y[2]);

			normalIndices.push_back(z[0]);
			normalIndices.push_back(z[1]);
			normalIndices.push_back(z[2]);
		}
	}

//	m_pVertices = new VERTEX[vertexIndices.size];

	for(int i = 0; i < vertexIndices.size(); i++)
	{
		
	}

	return 0;
}

void Mesh::LoadMeshFromHeader(ID3D11Device* device, const _OBJ_VERT_* vertArray, int vertexCount, const unsigned int* indexArray, int indexCount)
{
	mNumVertices = vertexCount ;
	mNumIndices = indexCount;

	m_pVertices = new VERTEX[mNumVertices];

	for(int i = 0; i < mNumVertices; i++)
	{
		m_pVertices[i].pos = XMFLOAT4{ vertArray[i].pos[0], vertArray[i].pos[1], vertArray[i].pos[2], 1.0f };
		m_pVertices[i].uv = XMFLOAT3{ vertArray[i].uvw[0], vertArray[i].uvw[1], vertArray[i].uvw[2]};
		m_pVertices[i].normal = XMFLOAT3{ vertArray[i].nrm[0], vertArray[i].nrm[1], vertArray[i].nrm[2]};
	}

	m_pIndices = new int[mNumIndices];

	for(int i = 0; i < mNumIndices; i++)
	{
		m_pIndices[i] = indexArray[i];
	}

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

void Mesh::LoadTexture(ID3D11Device* device, const wchar_t filename[])
{
	CreateDDSTextureFromFile(device, filename, (ID3D11Resource**)&m_pDiffuseMap.p, &m_pSRV.p);
}


int Mesh::RenderMesh(ID3D11DeviceContext* context, ID3D11VertexShader* VS, ID3D11PixelShader* PS, ID3D11InputLayout* inputLayout, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	if(m_pSRV.p != nullptr)
		context->PSSetShaderResources(0, 1, &m_pSRV.p);

	if(m_pSamplerState.p != nullptr)
		context->PSSetSamplers(0, 1, &m_pSamplerState.p);

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
