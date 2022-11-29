#include "DirectXBasis.h"
#include <cassert>

void DirectXBasis::Initialize(WinApp* winApp) {
	//null�`�F�b�N
	assert(winApp);
	winApp_ = winApp;

	//FPS�Œ菉����
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
	//�f�o�b�O�v���C���[���I����
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
	}
#endif

	HRESULT result;

	//DXGI�t�@�N�g���[����
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	//�A�_�v�^�[�̗񋓗p
	std::vector<ComPtr<IDXGIAdapter4>> adapters;

	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//�p�t�H�[�}���X���������̂��珇�ɁA���ׂẴA�_�v�^�[���
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND
		; i++)
	{
		//���I�z��ɒǉ�
		adapters.push_back(tmpAdapter);
	}

	//�Ó��ȃA�_�v�^��I��
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		//�A�_�v�^�[�̏����擾
		adapters[i]->GetDesc3(&adapterDesc);

		//�\�t�g�E�F�A�f�o�C�X�����
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//�f�o�C�X���̗p���ă��[�v�𔲂���
			tmpAdapter = adapters[i];
			break;
		}
	}

	//�Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device_));
		if (result == S_OK) {
			//�f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
}

void DirectXBasis::InitCommand() {
	HRESULT result;

	//�R�}���h�A���P�[�^�𐶐�
	result = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(result));

	//�R�}���h���X�g�𐶐�
	result = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(result));

	//�R�}���h�L���[��ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//�R�}���h�L���[�𐶐�
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));
}

void DirectXBasis::InitSwapChain() {
	HRESULT result;

	//IDXGISwapChain1��ComPtr��p��
	ComPtr<IDXGISwapChain1> swapChain1;

	//�X���b�v�`�F�[���̐ݒ�
	swapChainDesc_.Width = 1280;
	swapChainDesc_.Height = 720;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapChainDesc_.BufferCount = 2;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//�X���b�v�`�F�[���̐���
	result = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		winApp_->GetHwnd(),
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1);
	assert(SUCCEEDED(result));

	//��������IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�����
	swapChain1.As(&swapChain_);
}

void DirectXBasis::InitRTV() {
	//�f�X�N���v�^�q�[�v�̐ݒ�
	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount;
	//�f�X�N���v�^�q�[�v�̐���
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(&rtvHeap_));

	//�o�b�N�o�b�t�@
	backBuffers_.resize(swapChainDesc_.BufferCount);

	//�X���b�v�`�F�[���̂��ׂẴo�b�t�@�ɂ��ď�������
	for (size_t i = 0; i < backBuffers_.size(); i++) {
		//�X���b�v�`�F�[������o�b�t�@���擾
		swapChain_->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers_[i]));
		//�f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		//�����\���ŃA�h���X���Y����
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		//�����_�\�^�[�Q�b�g�r���[�̐ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//�V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//�����_�\�^�[�Q�b�g�r���[�̐���
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}
}

void DirectXBasis::InitDepthBuffer() {
	HRESULT result;

#pragma region �o�b�t�@�̃��\�[�X�ݒ�
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = WinApp::WinWidth;
	depthResourceDesc.Height = WinApp::WinHeight;
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
#pragma endregion

#pragma region ���̑��̐ݒ�
	//�[�x�l�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�x�l�N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
#pragma endregion

#pragma region �o�b�t�@�̐���
	//���\�[�X����
	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_)
	);
#pragma endregion

#pragma region �f�X�N���v�^�q�[�v�̐���
	//�[�x�r���[�p�f�X�N���v�^�q�[�v����
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
#pragma endregion

#pragma region �[�x�X�e���V���r���|�̐���
	//�[�x�r���|�쐬
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
	//�t�F���X�̐���
	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
}

void DirectXBasis::PrepareDraw() {
#pragma region ���\�[�X�o���A��`��\��Ԃ�
	//�o�b�N�o�b�t�@�̔ԍ����擾(0�Ԃ�1��)
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	//1.���\�[�X�o���A�ŏ������݉\�ɕύX
	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get();
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrierDesc_);
#pragma endregion

#pragma region �`����ύX
	//2.�`���̕ύX
	//�����_�\�^�[�Q�b�g�r���[�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
	//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	//�`�����w�肷��
	commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
#pragma endregion

#pragma region ��ʂ��N���A
	//3.��ʃN���A          R      G      B     A
	FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };
	commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	//�[�x�o�b�t�@�̃N���A�R�}���h��ǉ�
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
#pragma endregion

#pragma region �r���[�|�[�g�ݒ�
	//4.�`��R�}���h��������
	//�r���[�|�[�g�ݒ�R�}���h
	D3D12_VIEWPORT viewport{};
	viewport.Width = WinApp::WinWidth;			//����
	viewport.Height = WinApp::WinHeight;		//�c��
	viewport.TopLeftX = 0;						//����x
	viewport.TopLeftY = 0;						//����y
	viewport.MinDepth = 0.0f;					//�ŏ��[�x(0�ł悢)
	viewport.MaxDepth = 1.0f;					//�ő�[�x(1�ł悢)

	//�r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList_->RSSetViewports(1, &viewport);
#pragma endregion

#pragma region �V�U�[��`�ݒ�
	//�V�U�[��`
	D3D12_RECT scissorRect{};									//�؂蔲�����W
	scissorRect.left = 0;										//��
	scissorRect.right = scissorRect.left + WinApp::WinWidth;	//�E
	scissorRect.top = 0;										//��
	scissorRect.bottom = scissorRect.top + WinApp::WinHeight;	//��

	//�V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList_->RSSetScissorRects(1, &scissorRect);
#pragma endregion
}

void DirectXBasis::PostDraw() {
	HRESULT result;
	//4.�����܂ŁA�`��R�}���h

	//�o�b�N�o�b�t�@�̔ԍ����擾(0�Ԃ�1��)
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

#pragma region ���\�[�X�o���A��`��s��Ԃ�
	//5.���\�[�X�o���A��߂�
	barrierDesc_.Transition.StateBefore =
		D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierDesc_.Transition.StateAfter =
		D3D12_RESOURCE_STATE_PRESENT;
	commandList_->ResourceBarrier(1, &barrierDesc_);
#pragma endregion

#pragma region ���X�g�ɗ��߂Ă������R�}���h�����s
	//���߂̃N���[�Y
	result = commandList_->Close();
	assert(SUCCEEDED(result));
	//�R�}���h���X�g�̎��s
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
#pragma endregion

#pragma region ���s�����܂ő҂�
	//�R�}���h�̎��s������҂�
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

	//FPS�Œ�X�V
	fixFPS_->GetUpdate();

#pragma region �R�}���h���X�g�̃��Z�b�g
	//�A���P�[�^�[�����Z�b�g
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));

	//�R�}���h���X�g�����Z�b�g
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region �o�b�t�@���t���b�v
	//��ʂɕ\������o�b�t�@���t���b�v(���\�̓���ւ�)
	result = swapChain_->Present(1, 0);
	result = device_->GetDeviceRemovedReason();
	assert(SUCCEEDED(result));
#pragma endregion
}

DirectXBasis::~DirectXBasis() {
	delete fixFPS_;
	fixFPS_ = nullptr;
}