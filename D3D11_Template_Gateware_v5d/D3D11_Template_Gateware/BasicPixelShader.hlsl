struct OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : OCOLOR;
};

float4 main(OUTPUT input) : SV_TARGET
{
	return input.color;
}