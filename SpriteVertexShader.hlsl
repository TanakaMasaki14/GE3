#include"Sprite.hlsli"

VSOutput main(float4 pos : position, float2 uv : texcoord)
{
	VSOutput output;//ピクセルシェーダーに渡す値
	output.svpos = mul(mat, pos);
	output.uv = uv;
	return output;
}