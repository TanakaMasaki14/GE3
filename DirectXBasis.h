#pragma once
#include "WinApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include "FixFPS.h"

class DirectXBasis {

public: //namespaceの省略
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://基本的なメンバ関数

	//初期化
	void Initialize(WinApp* winApp);

private://固有のprivateメンバ関数

	//デバイス初期化
	void InitDevice();
	//コマンド初期化
	void InitCommand();
	//スワップチェーン初期化
	void InitSwapChain();
	//レンダ―ターゲットビュー初期化
	void InitRTV();
	//深度バッファ初期化
	void InitDepthBuffer();
	//フェンス初期化
	void InitFence();

public://固有のpublicメンバ関数

	//描画準備
	void PrepareDraw();
	//描画後処理
	void PostDraw();

public: //ゲッター

	//デバイス取得
	ID3D12Device* GetDevice() { return device_.Get(); }
	//コマンドリスト取得
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

private://よく使うメンバ変数

	//WindowsAPI
	WinApp* winApp_ = nullptr;

	///デバイス関連
	//DirectX12のデバイス
	ComPtr<ID3D12Device> device_ = nullptr;
	//DXGIファクトリ
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	///コマンド関連
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	///スワップチェーン関連
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

	///レンダ―ターゲットビュー関連
	//レンダ―ターゲットビューヒープ
	ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
	//レンダ―ターゲットビューデスク
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc_{};
	//バックバッファ
	std::vector<ComPtr<ID3D12Resource>> backBuffers_;

	//バリアデスク
	D3D12_RESOURCE_BARRIER barrierDesc_{};

	///深度バッファ関連
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff_ = nullptr;
	//深度バッファビューヒープ
	ComPtr<ID3D12DescriptorHeap> dsvHeap_ = nullptr;

	///フェンス関連
	//フェンス
	ComPtr<ID3D12Fence> fence_ = nullptr;
	//フェンス値
	UINT64 fenceVal_ = 0;

	///FPS固定
	FixFPS* fixFPS_ = nullptr;

public: //デストラクタ
	~DirectXBasis();
};