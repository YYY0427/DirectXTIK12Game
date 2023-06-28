//頂点シェーダ
struct Data {
	float4 svpos : SV_POSITION;//システムに渡す座標
	float4 col : COLOR;//頂点カラー
	float2 uv : TEXCOORD;//UV座標
};

cbuffer ConstantBuffer : register(b0) {
	matrix mat;
	float time;
};

Data main(float4 pos : POSITION, float4 norm : NORMAL, float2 uv : TEXCOORD)
{
	Data data;
	data.uv = uv;

	//SIMTなので、列優先で乗算される
	pos = mul(mat, pos);

	data.svpos = pos;
	data.col = (norm + 1.0) / 2.0;
	return data;
}