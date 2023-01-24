#pragma once
#include "DirectX.h"


class TextureManager
{
private:
	static int count;


	TextureManager();

	~TextureManager();


public:
	//リソース設定
	static D3D12_RESOURCE_DESC resDesc;
	//設定をもとにSRV用デスクリプタヒープを生成
	static ComPtr < ID3D12DescriptorHeap> srvHeap;
	static D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

	static const int srvCount = 512;
	static ComPtr<ID3D12Resource> texBuff[srvCount];

	//SRVの最大個数
	static const size_t kMaxSRVCount;
	//デスクリプタヒープの設定
	static D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;

	//デスクリプタレンジの設定
	static D3D12_DESCRIPTOR_RANGE descriptorRange;

public:
	//コピーコンストラクタを無効
	TextureManager(const TextureManager& obj) = delete;
	//代入演算子も
	TextureManager& operator=(const TextureManager& obj) = delete;

	static TextureManager& GetInstance();

	//デスクリプタヒープ初期化
	void InitializeDescriptorHeap();
	static void LoadGraph(const wchar_t* name, UINT64& textureHandle);
};

