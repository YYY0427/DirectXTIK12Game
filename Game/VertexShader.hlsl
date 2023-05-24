// ¸_VF[_
struct Data
{
	float4 svpos : SV_POSITION;	// VXeÉn·ÀW
	float4 col : COLOR;			// ¸_J[
	float2 uv : TEXCOORD;		// UVÀW
};

Data main(float4 pos : POSITION, float4 col : COLOR, float2 uv : TEXCOORD)
{
	Data data;
	data.uv = uv;

	// 2DsNZÀW©ç3DÀWÉÏ·
	// 0`640 ¨ -1`1
	// 0`480 ¨ 1`-1

	// c¡Ì¼ªÅé(YÍÌ)
	pos.xy *= float2(1.0 / 320.0, -1.0 / 240.0);

	// x = 0`2
	// y = -2`0
	// ãLð-1`1ÉÏ·
	// 0`2 - 1 = -1`1
	// -2`0 + 1 = -1`1 
	pos.xy -= float2(1, -1);

	data.svpos = pos;
	data.col = col;
	return data;
}