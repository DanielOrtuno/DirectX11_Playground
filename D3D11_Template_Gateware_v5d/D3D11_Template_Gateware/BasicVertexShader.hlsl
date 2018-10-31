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
	float time;
	bool dir;
	bool z;
};

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
};

OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;

	output.pos = input.pos;
	output.normal = mul(input.normal, worldMatrix);

	output.pos = mul(output.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projMatrix);

	if(enableTexture)
		output.color.xyz = input.uv;
	else
		output.color = color;

	output.useTexture = enableTexture;

	return output;
}