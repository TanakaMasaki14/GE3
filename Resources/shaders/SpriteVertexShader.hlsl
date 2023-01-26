#include"Sprite.hlsli"

VSOutput main( float4 pos : position ,float2 uv:texcoord)
{
	VSOutput output;//ピクセルシェーダーに渡す値
	output.svpos = mul(mat, pos);
	output.uv = uv;
	return output;

	//VSOutput output;//ピクセルシェーダーに渡す値
	//output.svpos = pos;
	//output.uv = uv;
	//return output;
}

//float4 main(float4 pos : POSITION,float2 uv : TEXCOORD) : SV_POSITION
//{
//	return pos;
//}