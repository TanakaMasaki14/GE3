#include"Sprite.hlsli"

VSOutput main( float4 pos : position ,float2 uv:texcoord)
{
	VSOutput output;//�s�N�Z���V�F�[�_�[�ɓn���l
	output.svpos = mul(mat, pos);
	output.uv = uv;
	return output;

	//VSOutput output;//�s�N�Z���V�F�[�_�[�ɓn���l
	//output.svpos = pos;
	//output.uv = uv;
	//return output;
}

//float4 main(float4 pos : POSITION,float2 uv : TEXCOORD) : SV_POSITION
//{
//	return pos;
//}