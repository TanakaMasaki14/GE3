#include "DirectXBasis.h"
#include <cassert>

void DirectXBasis::Initialize(WinApp* winApp) {
	//nullチェック
	assert(winApp);
	winApp_ = winApp;

	//FPS固定初期化
	fixFPS_ = new FixFPS();
	fixFPS_->GetInitialize();

#pragma region Initialize
	InitDevice();
	InitCommand();
	InitSwapChain();
	InitRTV();
	InitDepthBuffer();
	InitFence();
#pragma endregion
}

void DirectXBasis::InitDevice() {
#ifdef _DEBUG
	//デバッグプレイヤーをオンに
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
	}
#endif

	HRESULT result;

	//DXGIファクトリー生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	//アダプターの列挙用
	std::vector<ComPtr<IDXGIAdapter4>> adapters;

	//ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//パフォーマンスが高いものから順に、すべてのアダプターを列挙
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND
		; i++)
	{
		//動的配列に追加
		adapters.push_back(tmpAdapter);
	}

	//妥当なアダプタを選別
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		//アダプターの情報を取得
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//デバイスを採用してループを抜ける
			tmpAdapter = adapters[i];
			break;
		}
	}

	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device_));
		if (result == S_OK) {
			//デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
}

void DirectXBasis::InitCommand() {
	HRESULT result;

	//コマンドアロケータを生成
	result = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(result));

	//コマンドリストを生成
	result = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(result));

	//コマンドキューを設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));
}

void DirectXBasis::InitSwapChain() {
	HRESULT result;

	//IDXGISwapChain1のComPtrを用意
	ComPtr<IDXGISwapChain1> swapChain1;

	//スワップチェーンの設定
	swapChainDesc_.Width = 1280;
	swapChainDesc_.Height = 720;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapChainDesc_.BufferCount = 2;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//スワップチェーンの生成
	result = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		winApp_->GetHwnd(),
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1);
	assert(SUCCEEDED(result));

	//生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
	swapChain1.As(&swapChain_);
}

void DirectXBasis::InitRTV() {
	//デスクリプタヒープの設定
	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount;
	//デスクリプタヒープの生成
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(&rtvHeap_));

	//バックバッファ
	backBuffers_.resize(swapChainDesc_.BufferCount);

	//スワップチェーンのすべてのバッファについて処理する
	for (size_t i = 0; i < backBuffers_.size(); i++) {
		//スワップチェーンからバッファを取得
		swapChain_->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers_[i]));
		//デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		//裏か表化でアドレスがズレる
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		//レンダ―ターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダ―ターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}
}

void DirectXBasis::InitDepthBuffer() {
	HRESULT result;

#pragma region バッファのリソース設定
	//リソース設定
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = WinApp::WinWidth;
	depthResourceDesc.Height = WinApp::WinHeight;
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
#pragma endregion

#pragma region その他の設定
	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値クリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
#pragma endregion

#pragma region バッファの生成
	//リソース生成
	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_)
	);
#pragma endregion

#pragma region デスクリプタヒープの生成
	//深度ビュー用デスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
#pragma endregion

#pragma region 深度ステンシルビュ－の生成
	//深度ビュ－作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc,
		dsvHeap_->GetCPUDescriptorHandleForHeapStart()
	);
#pragma endregion
}

void DirectXBasis::InitFence() {
	HRESULT result;
	//フェンスの生成
	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
}

void DirectXBasis::PrepareDraw() {
#pragma region リソースバリアを描画可能状態に
	//バックバッファの番号を取得(0番か1番)
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	//1.リソースバリアで書き込み可能に変更
	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get();
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrierDesc_);
#pragma endregion

#pragma region 描画先を変更
	//2.描画先の変更
	//レンダ―ターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	//描画先を指定する
	commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
#pragma endregion

#pragma region 画面をクリア
	//3.画面クリア          R      G      B     A
	FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };
	commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	//深度バッファのクリアコマンドを追加
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
#pragma endregion

#pragma region ビューポート設定
	//4.描画コマンドここから
	//ビューポート設定コマンド
	D3D12_VIEWPORT viewport{};
	viewport.Width = WinApp::WinWidth;			//横幅
	viewport.Height = WinApp::WinHeight;		//縦幅
	viewport.TopLeftX = 0;						//左上x
	viewport.TopLeftY = 0;						//左上y
	viewport.MinDepth = 0.0f;					//最小深度(0でよい)
	viewport.MaxDepth = 1.0f;					//最大深度(1でよい)

	//ビューポート設定コマンドを、コマンドリストに積む
	commandList_->RSSetViewports(1, &viewport);
#pragma endregion

#pragma region シザー矩形設定
	//シザー矩形
	D3D12_RECT scissorRect{};									//切り抜き座標
	scissorRect.left = 0;										//左
	scissorRect.right = scissorRect.left + WinApp::WinWidth;	//右
	scissorRect.top = 0;										//上
	scissorRect.bottom = scissorRect.top + WinApp::WinHeight;	//下

	//シザー矩形設定コマンドを、コマンドリストに積む
	commandList_->RSSetScissorRects(1, &scissorRect);
#pragma endregion
}

void DirectXBasis::PostDraw() {
	HRESULT result;
	//4.ここまで、描画コマンド

	//バックバッファの番号を取得(0番か1番)
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

#pragma region リソースバリアを描画不可状態に
	//5.リソースバリアを戻す
	barrierDesc_.Transition.StateBefore =
		D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierDesc_.Transition.StateAfter =
		D3D12_RESOURCE_STATE_PRESENT;
	commandList_->ResourceBarrier(1, &barrierDesc_);
#pragma endregion

#pragma region リストに溜めておいたコマンドを実行
	//命令のクローズ
	result = commandList_->Close();
	assert(SUCCEEDED(result));
	//コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
#pragma endregion

#pragma region 実行完了まで待つ
	//コマンドの実行完了を待つ
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_) {
		_Post_ _Notnull_ HANDLE event = CreateEvent(nullptr, false, false, nullptr);

		if (event != 0)
		{
			fence_->SetEventOnCompletion(fenceVal_, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}
#pragma endregion

	//FPS固定更新
	fixFPS_->GetUpdate();

#pragma region コマンドリストのリセット
	//アロケーターをリセット
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));

	//コマンドリストをリセット
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region バッファをフリップ
	//画面に表示するバッファをフリップ(裏表の入れ替え)
	result = swapChain_->Present(1, 0);
	result = device_->GetDeviceRemovedReason();
	assert(SUCCEEDED(result));
#pragma endregion
}

DirectXBasis::~DirectXBasis() {
	delete fixFPS_;
	fixFPS_ = nullptr;
}