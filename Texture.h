#pragma once
#include<DirectXTex.h>
#include"DirectX.h"
#include<wrl.h>
#include<array>
#include<string>

class Texture {
public:
	static const size_t spriteSRVCount = 2048;
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;			//デスクリプタヒープ
	static std::array< Microsoft::WRL::ComPtr<ID3D12Resource>, spriteSRVCount >texBuffuers;	//テクスチャバッファ
	static D3D12_RESOURCE_DESC textureResourceDesc;
	static ID3D12Device* dev;


public:
	static uint32_t LoadTexture(const wchar_t* fileName = L"NULL");
	static void Initialize(ID3D12Device* device);
	static void CreateSRV(ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle);
};

