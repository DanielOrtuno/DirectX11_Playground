textureCUBE diffuseMap : register( t0 );
SamplerState filter : register( s0 );

struct OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : OCOLOR;
};

float4 main(OUTPUT input) : SV_TARGET
{
	return diffuseMap.Sample(filter, input.color.xyz);
}