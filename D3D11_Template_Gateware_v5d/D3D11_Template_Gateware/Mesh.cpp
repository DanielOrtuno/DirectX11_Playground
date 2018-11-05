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

void Mesh::CreateBuffers(ID3D11Device* device)
{
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

	
	CreateBuffers(device);

}

int Mesh::CreateSkybox(ID3D11Device* device)
{
	mNumVertices = 8;
	mNumIndices = 36;
	m_pVertices = new VERTEX[mNumVertices];
	m_pIndices = new int[mNumIndices];
	int vertexIndex = 0;

	for(int i = -1; i <= 1; i+= 2)
	{
		m_pVertices[vertexIndex++] = VERTEX{ XMFLOAT4{ -1, -1, (float)i,  1 } }; //bottom left
		m_pVertices[vertexIndex++] = VERTEX{ XMFLOAT4{ -1,  1, (float)i,  1 } }; //top left
		m_pVertices[vertexIndex++] = VERTEX{ XMFLOAT4{  1,  1, (float)i,  1 } }; //top right
		m_pVertices[vertexIndex++] = VERTEX{ XMFLOAT4{  1, -1, (float)i,  1 } }; //bottom right
	}
	
	/*m_pIndices[2], m_pIndices[5], m_pIndices[21], m_pIndices[32], m_pIndices[35] = 0;

	m_pIndices[1], m_pIndices[18], m_pIndices[22], m_pIndices[26], m_pIndices[29] = 1;

	m_pIndices[0], m_pIndices[4], m_pIndices[7], m_pIndices[23], m_pIndices[27] = 2;

	m_pIndices[3], m_pIndices[8], m_pIndices[11], m_pIndices[33] = 3;

	m_pIndices[15], m_pIndices[20], m_pIndices[31] = 4;

	m_pIndices[12], m_pIndices[16], m_pIndices[25] = 5;

	m_pIndices[6], m_pIndices[10], m_pIndices[13], m_pIndices[19], m_pIndices[24], m_pIndices[28] = 6;

	m_pIndices[9], m_pIndices[14], m_pIndices[17], m_pIndices[30], m_pIndices[34] = 7;*/


	CreateBuffers(device);

	return 0;
}

void Mesh::InitializeAs3DGrid(ID3D11Device* device)
{
	std::vector<VERTEX> list;

	float pos = -1;

	for(pos; pos < 1; pos += 0.1f)
	{
		VERTEX newVertex;

		if(rand() % 2 == 0)
			newVertex = VERTEX{XMFLOAT4(pos, 0.0f, -1, 1), XMFLOAT3(9,0,0) };
		else
			newVertex = VERTEX{ XMFLOAT4(pos, 0.0f, -1, 1)};

		list.push_back(newVertex);

		if(rand() % 2 == 0)
			newVertex = VERTEX{ XMFLOAT4(pos, 0.0f, .9f, 1), XMFLOAT3(9,0,0) };
		else
			newVertex = VERTEX{ XMFLOAT4(pos, 0.0f, .9f, 1) };

		list.push_back(newVertex);

		if(rand() % 2 == 0)
			newVertex = VERTEX{ XMFLOAT4(-1, 0.0f, pos, 1), XMFLOAT3(9,0,0) };
		else
			newVertex = VERTEX{ XMFLOAT4(-1, 0.0f, pos, 1) };

		list.push_back(newVertex);

		if(rand() % 2 == 0)
			newVertex = VERTEX{ XMFLOAT4(.9f, 0.0f, pos, 1), XMFLOAT3(9,0,0) };
		else
			newVertex = VERTEX{ XMFLOAT4(.9f, 0.0f, pos, 1) };

		list.push_back(newVertex);

	}

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

int Mesh::LoadMeshFromFile(ID3D11Device* device, const char filename[])
{
	FILE* file = fopen(filename, "r");

	if(file == NULL)
		return 1;
	
	std::vector<XMFLOAT4> temp_vertices;
	std::vector<XMFLOAT3> temp_uv;
	std::vector<XMFLOAT3> temp_normals;

	std::vector<int>	  vertexIndices;
	std::vector<int>	  uvIndices;
	std::vector<int>	  normalIndices;

	
	//Read file 
	while(1)
	{
		char buffer[128];
		int result = fscanf(file, "%s", buffer);

		if(result == EOF)
			break;

		if(strcmp(buffer, "v") == 0)
		{
			XMFLOAT4 newData;
			fscanf(file, "%f %f %f\n", &newData.x, &newData.y, &newData.z);

			newData.w = 1.0f;
			temp_vertices.push_back(newData);
		}
		else if(strcmp(buffer, "vt") == 0)
		{
			XMFLOAT3 newData;
			fscanf(file, "%f %f\n", &newData.x, &newData.y);
			newData.y = 1 - newData.y;
			newData.z = 0;
			temp_uv.push_back(newData);
		}
		else if(strcmp(buffer, "vn") == 0)
		{
			XMFLOAT3 newData;
			fscanf(file, "%f %f %f\n", &newData.x, &newData.y, &newData.z);

			temp_normals.push_back(newData);
		}
		else if(strcmp(buffer, "f") == 0)
		{
			int vert[3], texcoor[3], norm[3];

 			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vert[0], &texcoor[0], &norm[0], &vert[1], &texcoor[1], &norm[1], &vert[2], &texcoor[2], &norm[2]);
			if(matches != 9) 
				return 2;
			
			vertexIndices.push_back(vert[0]);
			vertexIndices.push_back(vert[1]);
			vertexIndices.push_back(vert[2]);

			uvIndices.push_back(texcoor[0]);
			uvIndices.push_back(texcoor[1]);
			uvIndices.push_back(texcoor[2]);

			normalIndices.push_back(norm[0]);
			normalIndices.push_back(norm[1]);
			normalIndices.push_back(norm[2]);
		}

	}


	//Check for duplicates 
	std::unordered_map<VERTEX, int, hashFct_VERTEX> umap;

	std::vector<int> finalIndexList;
	int	indexer = 0;

	for(unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		VERTEX newVertex = VERTEX{ temp_vertices[vertexIndices[i] - 1], temp_uv[uvIndices[i] - 1], temp_normals[normalIndices[i] - 1] };

		if(umap.count(newVertex) == 0)
		{
			// New Vertex. Add it to the array
			finalIndexList.push_back(indexer);
			umap.insert({ newVertex, indexer++ } );
		}
		else
		{
			// Vertex is already in the array. Just add the index
			finalIndexList.push_back(umap[newVertex]);
		}
	}

	//Put everything in the arrays 

	mNumVertices = umap.size();
	m_pVertices = new VERTEX[mNumVertices];

	for(int i = 0; i < mNumVertices; i++)
	{
		auto it = std::find_if(umap.begin(), umap.end(), [&i](const std::pair<VERTEX, int> &p)
		{
			return p.second == i;
		});

		m_pVertices[i] = it->first;
	}

	mNumIndices = finalIndexList.size();

	m_pIndices = new int[mNumIndices];
	
	for(int i = 0; i < mNumIndices; i++)
	{
		m_pIndices[i] = finalIndexList[i];
	}


	CreateBuffers(device);

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

	CreateBuffers(device);

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
