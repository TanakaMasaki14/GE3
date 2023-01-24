#pragma once
#include"TextureManager.h"

class ConstBuffTransform
{
private:
	//定数バッファ用データ構造
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj; //ビュープロジェクション行列
		XMMATRIX world;    //ワールド行列
		XMFLOAT3 cameraPos;//カメラ座標（ワールド座標）
	};

public:
	ComPtr < ID3D12Resource> constBuffTransform = nullptr;//定数バッファのGPUリソースのポインタ
	ConstBufferDataTransform* constMapTransform = nullptr;//定数バッファのマッピング用ポインタ

	ConstBuffTransform(/*D3D12_RESOURCE_DESC& resDesc,*/ /*Directx& directx*/);
	void Initialize(Directx& directx);
};