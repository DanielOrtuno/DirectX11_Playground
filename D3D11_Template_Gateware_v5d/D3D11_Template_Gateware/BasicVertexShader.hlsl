
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
	output.color = input.color;

	return output;
}