struct Data
{
	float4 svpos : SV_POSITION;	// �V�X�e���ɓn�����W
	float4 col : COLOR;			// ���_�J���[
	float2 uv : TEXCOORD;		// UV���W
};

float4 main(Data data) : SV_TARGET
{
	
	return float4(data.uv, 1, 1);
}