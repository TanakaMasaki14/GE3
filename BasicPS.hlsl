#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	//�E���� �����̃��C�g
	float3 light = normalize(float3(1,-1,1));

	//diffuse��[0,1]�͈̔͂�Clamp����
	//�����ւ̃x�N�g���Ɩ@���x�N�g���̓���
	float diffuse = saturate(dot(-light, input.normal));

	//�A���r�G���g����0.3�Ƃ��Čv�Z
	float brightness = diffuse + 0.3f;

	//�e�N�X�`���̐F
	float4 texColor = float4(tex.Sample(smp, input.uv));


	//�P�x��RGB�ɑ�����ďo��
	//�e�N�X�`���̐F�ƍ�������
	return float4(texColor.rgb * brightness,texColor.a) * color;


	//RGB�����ꂼ��̖@����XYZ�AA��1�ŏo��
	//float4(input.normal,1);
}