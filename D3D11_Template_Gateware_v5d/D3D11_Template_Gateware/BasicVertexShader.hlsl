#pragma pack_matrix(row_major)

cbuffer cameraBuffer : register( b0 )
{
	float4x4 viewMatrix;
	float4x4 projMatrix;
};

cbuffer meshBuffer : register( b1 )
{
	float4x4 worldMatrix;
};

struct VERTEX
{
	float4 pos : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

struct OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : OCOLOR;
};

OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;

	output.pos = input.pos;

	output.pos = mul(output.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projMatrix);

	output.color = input.color;

	return output;
}