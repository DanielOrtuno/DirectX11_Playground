Texture2D diffuseMap : register( t0 );
SamplerState filter : register( s0 );

cbuffer lightBuffer : register( b0 )
{
	float4 lightDirection[2];
	float4 lightColor[2];

	float4 pointlightPos;
	float4 pointlightColor;

	float4 spotlightPos;
	float4 spotlightConeDir;
	float4 spotlightColor;
}

struct OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : OCOLOR0;
	float4 worldPos : POSITION;
	float4 tintColor : OCOLOR1;
	bool useTexture : BOOL;
};

float4 main(OUTPUT input) : SV_TARGET
{
	float4 finalColor = 0;

	float4 val;

	if(input.useTexture)
		val = diffuseMap.Sample(filter, input.color.xy);
	else
		val = input.color;

	float lightRatio;

	//Convert to gray scale
	float gray = (0.2126f) * (val.r * 255) + (0.7152f) * ( val.g * 255 ) + (0.0722f * ( val.b * 255 ));
	
	if(gray > 128)
		val *= input.tintColor;

	////Directional lights
	for(int i = 0; i < 2; i++)
	{
		lightRatio = clamp(dot(-normalize(lightDirection[i].xyz), input.normal), 0, 1);

		lightRatio = clamp(lightRatio + .15f, 0, 1);
		finalColor += lightRatio * lightColor[i] * val;
	}

	////Point light

	float4 toLight = pointlightPos - input.worldPos;
	float3 lightLength = length(toLight);

	float rangeAttenuation = 1 - clamp(lightLength / 5.0f, 0, 1);

	float angularAttenuation = clamp(dot(toLight / lightLength, input.normal), 0, 1);

	finalColor += pointlightColor * rangeAttenuation * angularAttenuation;

	////Spotlight 

	float spotlightOuterRatio = .85f;
	float spotlightInnerRatio = .99f;

	float4 spotlightDir = normalize(spotlightPos - input.worldPos);
	float surfaceRatio = saturate(dot(-spotlightDir, spotlightConeDir));
	float spotFactor = saturate(( surfaceRatio - spotlightOuterRatio ) / ( spotlightInnerRatio - spotlightOuterRatio ));

	lightRatio = saturate(dot(spotlightDir.xyz, input.normal));

	finalColor += spotFactor * lightRatio * spotlightColor;
	return finalColor;
}