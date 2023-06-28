
struct Data {
	float4 svpos : SV_POSITION;//�V�X�e���ɓn�����W
	float4 col : COLOR;//���_�J���[
	float2 uv : TEXCOORD;//UV���W
};

//�e�N�X�`���I�u�W�F�N�g
Texture2D tex : register(t0);
Texture2D norm : register(t1);
SamplerState smp : register(s0);

//�萔�o�b�t�@
cbuffer ConstantBuffer : register(b0) {
	matrix mat;
	float time;
};

float4 main(Data data) : SV_TARGET
{
	float angle = time * 2 * 3.14159265358;
	float3 light = float3(cos(angle),sin(angle),1);//�E�����Ɍ������x�N�g��
//	float3 light = float3(1, 1, 1);//�E�����Ɍ������x�N�g��
	light = normalize(light);//���K��

	//�@���}�b�v���󂯎��(�������͈͂�0�`1�Ȃ̂ŁA-1�`1�ɕϊ����K�v)
	float3 n = norm.Sample(smp,data.uv);
	n.xy *= 2;//2�{(0�`2)
	n.xy -= 1;//-1�`1
	n.z = -sqrt(1 - n.x * n.x - n.y * n.y);//Z�v�Z

	float4 color = tex.Sample(smp, data.uv);

	if (color.a == 0.0) {
		discard;//���̓h��Ԃ��𖳂��������Ƃɂ���
	}

	//�f�B�t���[�Y(���邳�v�Z)
	float bright = saturate(dot(n, -light));
	color.rgb *= bright;//�ݒ肳��Ă�F�Ƀf�B�t���[�Y����Z����

	//�X�y�L�����[(�n�C���C�g�F����͔��F�������̂܂ܔ��˂��Ă�ƍl����)
	light = reflect(light, n);//���˃x�N�g��
	float3 eyeRay = float3(0, 0, 1);//�����x�N�g��
	float spec = pow(saturate(dot(light, -eyeRay)), 10);
	color.rgb += spec;//���ۂɂ�spec*float3(1�C1�C1)�����Z����Ă���

	//if (distance(data.uv, float2(0.5, 0.5)) < 0.4) {
		//color.rgb = dot(color.rgb, float3(0.288, 0.587, 0.144));
	//}
	return color;

	//return float4(data.uv,1,1);
}