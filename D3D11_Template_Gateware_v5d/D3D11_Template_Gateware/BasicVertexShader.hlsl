#pragma pack_matrix(row_major)

cbuffer cameraBuffer : register( b0 )
{
	float4x4 viewMatrix;
	float4x4 projMatrix;
};

cbuffer meshBuffer : register( b1 )
{
	float4x4 worldMatrix;
	float4 color;
	bool enableTexture;
	bool x;
	bool y;
	bool z;
};

cbuffer lightBuffer : register( b2 )
{
	float4 lightDirection[2];
	float4 lightColor[2];
}

struct VERTEX
{
	float4 pos : POSITION;
	float3 uv : TEXTURECOORD;
	float3 normal : NORMAL;
};

struct OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : OCOLOR;
	bool useTexture : BOOL;
	float4 lightDirection[2] : POSITION;
	float4 lightColor[2] : COLOR;
};

OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;

	output.pos = input.pos;
	output.normal = input.normal;

	output.pos = mul(output.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projMatrix);

	output.useTexture = enableTexture;

	if(enableTexture)
		output.color.xyz = input.uv;
	else
		output.color = color;
	
	for(int i = 0; i < 2; i++)
	{
		output.lightDirection[i] = lightDirection[i];
		output.lightColor[i] = lightColor[i];
	}

	return output;
}