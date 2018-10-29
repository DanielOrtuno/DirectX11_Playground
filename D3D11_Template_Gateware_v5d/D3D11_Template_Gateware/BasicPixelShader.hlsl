Texture2D diffuseMap : register( t0 );
SamplerState filter : register( s0 );


struct OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : OCOLOR;
	bool useTexture : BOOL;
	float4 lightDirection[2] : POSITION;
	float4 lightColor[2] : COLOR;
};

float4 main(OUTPUT input) : SV_TARGET
{

	if(input.useTexture)
	{
		float3 uv = input.color.xyz;
		float4 finalColor = 0;

		for(int i = 0; i < 2; i++)
		{
			float lightRatio = clamp(dot(-input.lightDirection[i].xyz, input.normal), 0, 1);

			lightRatio = clamp(lightRatio + .15f, 0, 1);
			finalColor += lightRatio * input.lightColor[i] * diffuseMap.Sample(filter, uv);;
		}

		return finalColor;
	}
	else
		return input.color;
}