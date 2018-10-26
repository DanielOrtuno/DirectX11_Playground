#pragma pack_matrix(row_major)

cbuffer cb : register( b0 )
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
};

struct VERTEX
{
	float4 pos : POSITION;
	float4 color : COLOR;
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