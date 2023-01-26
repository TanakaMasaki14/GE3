//マテリアル
//cbuffer cbuff0 : register(b0)
//{
//	float4 color;
//	matrix mat;
//};

//マテリアル
cbuffer constBufferDataMaterial : register(b0)
{
	float4 color;
	matrix mat;
};

//頂点シェーダ―の出力構造体
struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};