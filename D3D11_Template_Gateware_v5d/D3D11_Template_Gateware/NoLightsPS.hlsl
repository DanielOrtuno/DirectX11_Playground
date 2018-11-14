Texture2D diffuseMap : register( t0 );
SamplerState filter : register( s0 );

struct OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : OCOLOR;
	float4 worldPos : POSITION;
	bool useTexture : BOOL;
};

float4 main(OUTPUT input) : SV_TARGET
{
	float4 finalColor = 0;

	if(input.useTexture)
		finalColor = diffuseMap.Sample(filter, input.color.xy);
	else
		finalColor = input.color;

	return finalColor/3;
}