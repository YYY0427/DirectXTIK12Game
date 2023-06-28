
struct Data {
	float4 svpos : SV_POSITION;//システムに渡す座標
	float4 col : COLOR;//頂点カラー
	float2 uv : TEXCOORD;//UV座標
};

//テクスチャオブジェクト
Texture2D tex : register(t0);
Texture2D norm : register(t1);
SamplerState smp : register(s0);

//定数バッファ
cbuffer ConstantBuffer : register(b0) {
	matrix mat;
	float time;
};

float4 main(Data data) : SV_TARGET
{
	float angle = time * 2 * 3.14159265358;
	float3 light = float3(cos(angle),sin(angle),1);//右下奥に向かうベクトル
//	float3 light = float3(1, 1, 1);//右下奥に向かうベクトル
	light = normalize(light);//正規化

	//法線マップを受け取る(ただし範囲が0～1なので、-1～1に変換が必要)
	float3 n = norm.Sample(smp,data.uv);
	n.xy *= 2;//2倍(0～2)
	n.xy -= 1;//-1～1
	n.z = -sqrt(1 - n.x * n.x - n.y * n.y);//Z計算

	float4 color = tex.Sample(smp, data.uv);

	if (color.a == 0.0) {
		discard;//この塗りつぶしを無かったことにする
	}

	//ディフューズ(明るさ計算)
	float bright = saturate(dot(n, -light));
	color.rgb *= bright;//設定されてる色にディフューズを乗算する

	//スペキュラー(ハイライト：今回は白色光をそのまま反射してると考える)
	light = reflect(light, n);//反射ベクトル
	float3 eyeRay = float3(0, 0, 1);//視線ベクトル
	float spec = pow(saturate(dot(light, -eyeRay)), 10);
	color.rgb += spec;//実際にはspec*float3(1，1，1)が加算されている

	//if (distance(data.uv, float2(0.5, 0.5)) < 0.4) {
		//color.rgb = dot(color.rgb, float3(0.288, 0.587, 0.144));
	//}
	return color;

	//return float4(data.uv,1,1);
}