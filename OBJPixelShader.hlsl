
#include "OBJShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[


float4 main(VSOutput input) : SV_TARGET
{
	// �e�N�X�`���}�b�s���O
		float4 texcolor = tex.Sample(smp, input.uv);

		// ����x
		const float shininess = 4.0f;
		// ���_���王�_�ւ̕����x�N�g��
		float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

		// �����ˌ�
		float3 ambient = m_ambient;

		// �V�F�[�f�B���O�ɂ��F
		float4 shadecolor = float4(ambientColor * ambient, m_alpha);

		//���s����
		for (int i = 0; i < DIRLIGHT_NUM; i++) {
			if (dirLights[i].active) {
				// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
				float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);
				// ���ˌ��x�N�g��
				float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
				// �g�U���ˌ�
				float3 diffuse = dotlightnormal * m_diffuse;
				// ���ʔ��ˌ�
				float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

				// �S�ĉ��Z����
				shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
			}
		}

		//�_����
		for (int i = 0; i < POINTLIGHT_NUM; i++) {
			if (pointLights[i].active) {
				//���C�g�ւ̃x�N�g��
				float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
				//�x�N�g���̒���
				float d = length(lightv);
				//���K�����A�P�ʃx�N�g���ɂ���
				lightv = normalize(lightv);
				//���������W��
				float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d +
					pointLights[i].lightatten.z * d * d);
				// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
				float3 dotlightnormal = dot(lightv, input.normal);
				// ���ˌ��x�N�g��
				float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
				// �g�U���ˌ�
				float3 diffuse = dotlightnormal * m_diffuse;
				// ���ʔ��ˌ�
				float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

				// �S�ĉ��Z����
				shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
			}
		}

		// �V�F�[�f�B���O�ɂ��F�ŕ`��
		return shadecolor * texcolor * color;
}