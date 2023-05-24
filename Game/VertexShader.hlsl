// 頂点シェーダ
struct Data
{
	float4 svpos : SV_POSITION;	// システムに渡す座標
	float4 col : COLOR;			// 頂点カラー
	float2 uv : TEXCOORD;		// UV座標
};

Data main(float4 pos : POSITION, float4 col : COLOR, float2 uv : TEXCOORD)
{
	Data data;
	data.uv = uv;

	// 2Dピクセル座標から3D座標に変換
	// 0～640 → -1～1
	// 0～480 → 1～-1

	// 縦横の半分で割る(Yは負の数)
	pos.xy *= float2(1.0 / 320.0, -1.0 / 240.0);

	// x = 0～2
	// y = -2～0
	// 上記を-1～1に変換
	// 0～2 - 1 = -1～1
	// -2～0 + 1 = -1～1 
	pos.xy -= float2(1, -1);

	data.svpos = pos;
	data.col = col;
	return data;
}