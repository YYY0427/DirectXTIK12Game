struct Data
{
	float4 svpos : SV_POSITION;	// システムに渡す座標
	float4 col : COLOR;			// 頂点カラー
	float2 uv : TEXCOORD;		// UV座標
};

float4 main(Data data) : SV_TARGET
{
	
	return float4(data.uv, 1, 1);
}