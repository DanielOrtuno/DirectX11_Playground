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
	float4 worldPos : POSITION;
	bool useTexture : BOOL;
};

OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;

	output.pos = input.pos;


	if(input.uv.x == 9)
	{
		if(dir)
		{
			output.pos.y += sin(time);
			output.color = float4( 0, 1, 1, 1 );
		}
		else
		{
			output.pos.y -= sin(time);
			output.color = float4( 1, 1, 1, 1 );

		}
	}	
	else
		output.color = color;

	output.normal = input.normal;

	output.pos = mul(output.pos, worldMatrix);
	output.worldPos = output.pos;
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projMatrix);



	output.useTexture = enableTexture;



	return output;
};

