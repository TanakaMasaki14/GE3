#pragma once
#include"ConstBuffTransform.h"
#include"TextureManager.h"
#include "WorldMat.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
using namespace Microsoft::WRL;//


struct PipeLineSet
{
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12RootSignature> rootSignature;
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
};
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;//色(RGBA)
};
struct SpriteBuff
{
	//頂点バッファ
	ComPtr < ID3D12Resource> vertBuff = nullptr;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//定数バッファの生成
};
struct VertexSprite
{
	XMFLOAT3 pos;//xyz座標
	XMFLOAT2 uv;//uv座標
};

class SpriteCommon
{
private:


public:
	PipeLineSet pipelineSet;
};

