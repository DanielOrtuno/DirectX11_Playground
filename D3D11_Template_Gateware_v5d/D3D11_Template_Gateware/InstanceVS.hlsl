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
	float4 instData : INSTANCEPOS;
};

struct OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : OCOLOR;
	float4 worldPos : POSITION;
	bool useTexture : BOOL;
};

OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;

	float rot = input.instData.w * 3.14 / 180;

	float4x4 rotMatrix = { cos(rot),	0,	 sin(rot),	0,
								0,		0,		0	,	0,
							-sin(rot),	0,	 cos(rot),  0,
								0	,	0,		0	,	1 };

	input.pos.x += input.instData.x;
	input.pos.y += input.instData.y;
	input.pos.z += input.instData.z;

	output.pos = input.pos;

	output.normal = mul(input.normal, worldMatrix);
	//output.normal = mul(output.normal, rotMatrix);

	output.pos = mul(output.pos, worldMatrix);
	//output.pos = mul(output.pos, rotMatrix);

	output.worldPos = output.pos;

	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projMatrix);

	output.color.xyz = input.uv;
	
	output.useTexture = true;
	return output;
}