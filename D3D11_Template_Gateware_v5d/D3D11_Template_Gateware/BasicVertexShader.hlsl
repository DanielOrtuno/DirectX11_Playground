
struct VERTEX
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct OUTPUT
{
	float4 pos : SV_POSITION;

};

OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;

	output.pos = input.pos;

	return output;
}