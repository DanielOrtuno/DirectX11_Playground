struct OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : OCOLOR;
};

float4 main() : SV_TARGET
{
	return float4(1,0,0,1);
}