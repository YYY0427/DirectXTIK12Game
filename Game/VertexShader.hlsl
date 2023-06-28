//���_�V�F�[�_
struct Data {
	float4 svpos : SV_POSITION;//�V�X�e���ɓn�����W
	float4 col : COLOR;//���_�J���[
	float2 uv : TEXCOORD;//UV���W
};

cbuffer ConstantBuffer : register(b0) {
	matrix mat;
	float time;
};

Data main(float4 pos : POSITION, float4 norm : NORMAL, float2 uv : TEXCOORD)
{
	Data data;
	data.uv = uv;

	//SIMT�Ȃ̂ŁA��D��ŏ�Z�����
	pos = mul(mat, pos);

	data.svpos = pos;
	data.col = (norm + 1.0) / 2.0;
	return data;
}