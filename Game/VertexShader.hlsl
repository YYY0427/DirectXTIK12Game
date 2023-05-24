// ���_�V�F�[�_
struct Data
{
	float4 svpos : SV_POSITION;	// �V�X�e���ɓn�����W
	float4 col : COLOR;			// ���_�J���[
	float2 uv : TEXCOORD;		// UV���W
};

Data main(float4 pos : POSITION, float4 col : COLOR, float2 uv : TEXCOORD)
{
	Data data;
	data.uv = uv;

	// 2D�s�N�Z�����W����3D���W�ɕϊ�
	// 0�`640 �� -1�`1
	// 0�`480 �� 1�`-1

	// �c���̔����Ŋ���(Y�͕��̐�)
	pos.xy *= float2(1.0 / 320.0, -1.0 / 240.0);

	// x = 0�`2
	// y = -2�`0
	// ��L��-1�`1�ɕϊ�
	// 0�`2 - 1 = -1�`1
	// -2�`0 + 1 = -1�`1 
	pos.xy -= float2(1, -1);

	data.svpos = pos;
	data.col = col;
	return data;
}