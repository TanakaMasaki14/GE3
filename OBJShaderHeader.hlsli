
//�}�e���A��
cbuffer ConstBufferDataMaterial : register(b0)
{
	float4 color;//�F(RGBA)
}

//3D�ϊ��s��
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix viewproj; //�r���[�v���W�F�N�V�����s��
	matrix world;    //���[���h�s��
	float3 cameraPos;//�J�������W�i���[���h���W�j
}

cbuffer ConstBufferDataMaterial2 : register(b2)
{
	float3 m_ambient  : packoffset(c0);//�A���r�G���g
	float3 m_diffuse  : packoffset(c1);//�f�B�t���[�Y
	float3 m_specular : packoffset(c2);//�X�y�L�����[
	float  m_alpha : packoffset(c2.w);//�A���t�@
}

// �_�����̐�
static const int POINTLIGHT_NUM = 3;
struct PointLight
{
	float3 lightpos;//���C�g�̍��W
	float3 lightcolor;//���C�g�̐F
	float3 lightatten;//���C�g�̋��������W��
	uint   active;//�L����
};

// ���s�����̐�
static const int DIRLIGHT_NUM = 3;
struct DirLight
{
	float3 lightv;    // ���C�g�ւ̕����̒P�ʃx�N�g��
	float3 lightcolor;    // ���C�g�̐F(RGB)
	uint   active;      //�L����
};

//LightManager��ConstBuffer�Ɠ����^��錾
cbuffer ConstBufferDataMaterial3 : register(b3)
{
	float3     ambientColor;
	DirLight   dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
}

//���_�V�F�[�_����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos    : SV_POSITION;//�V�X�e���p���_���W
	float4 worldpos : POSITION;//���[���h���W
	float3 normal   : NORMAL;//���[���h���W
	float2 uv       : TEXCOORD;//uv���W
};