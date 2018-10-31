Texture2D diffuseMap : register( t0 );
SamplerState filter : register( s0 );

cbuffer lightBuffer : register( b0 )
{
	float4 lightDirection[2];
	float4 lightColor[2];	
	float4 pointlightPos;
	float4 pointlightColor;
}

struct OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : OCOLOR;
	bool useTexture : BOOL;
};

float4 main(OUTPUT input) : SV_TARGET
{
	float4 finalColor = 0;

	float4 val;


	if(input.useTexture)
		val = diffuseMap.Sample(filter, input.color);
	else
		val = input.color;

	float lightRatio;

	//Directional lights
	for(int i = 0; i < 2; i++)
	{
		lightRatio = clamp(dot(-normalize(lightDirection[i].xyz), input.normal), 0, 1);

		lightRatio = clamp(lightRatio + .15f, 0, 1);
		finalColor += lightRatio * lightColor[i] * val;
	}

	//Point light
	//LIGHTDIR = NORMALIZE(LIGHTPOS – SURFACEPOS)
	//LIGHTRATIO = CLAMP(DOT(LIGHTDIR, SURFACENORMAL))
	//RESULT = LIGHTRATIO * LIGHTCOLOR * SURFACECOLOR

	//float attenuation = 1 - clamp(length(pointlightPos - input.pos) / .5f, 0 ,1);


	//float4 pointlightDir = normalize(pointlightPos - input.pos);
	//lightRatio = clamp(dot(pointlightDir, input.normal), 0, 1);


	//finalColor += lightRatio * pointlightColor * finalColor;
	return finalColor;
}