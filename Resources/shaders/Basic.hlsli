//�}�e���A��
cbuffer ConstBufferData:register(b0)
{
	float4 color;//�F(RGBA)
	matrix mat; //3D�ϊ��s��
}

//���_�V�F�[�_�[�̏o�͍\����
//(���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����)
struct VSOutput
{
	//�V�X�e���p���_���W
	float4 svpos : SV_POSITION;
	//�@���x�N�g��
	float3 normal :NORMAL;
	//uv�l
	float2 uv :TEXCOORD;
};

//3D�ϊ��s��
//cbuffer ConstBufferDataTransform : register(b1)
//{
//
//};