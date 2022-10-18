cbuffer Transform : register(b0){
	float4x4 World:packoffset(c0); // ���[���h�s��
	float4x4 View:packoffset(c4); // �r���[�s��
	float4x4 Proj:packoffset(c8); // ���e�s��
}

struct VSInput{
	float3 pos : POSITION; // ���_���W
	float3 normal : NORMAL; // �@��
	float2 uv : TEXCOORD; // UV
	float3 tangent : TANGENT; // �ڋ��
	float4 color : COLOR; // ���_�F
};

struct VSOutput{
	float4 svpos : SV_POSITION; // �ϊ����ꂽ���W
	float4 color : COLOR; // �ϊ����ꂽ�F
	float2 uv : TEXCOORD; // �s�N�Z���V�F�[�_�[��uv��n��
};


VSOutput vert(VSInput input){
	VSOutput output = (VSOutput)0; // �A�E�g�v�b�g�\���̂��`����

	float4 localPos = float4(input.pos, 1.0f); // ���_���W
	float4 worldPos = mul(World, localPos); // ���[���h���W�ɕϊ�
	float4 viewPos = mul(View, worldPos); // �r���[���W�ɕϊ�
	float4 projPos = mul(Proj, viewPos); // ���e�ϊ�

	output.svpos = projPos; // ���e�ϊ����ꂽ���W���s�N�Z���V�F�[�_�[�ɓn��
	output.color = input.color; // ���_�F�����̂܂܃s�N�Z���V�F�[�_�[�ɓn��
	output.uv = input.uv; // ���͂���uv��n��

	return output;
}