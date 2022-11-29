#pragma once
#include "WinApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include "FixFPS.h"

class DirectXBasis {

public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://��{�I�ȃ����o�֐�

	//������
	void Initialize(WinApp* winApp);

private://�ŗL��private�����o�֐�

	//�f�o�C�X������
	void InitDevice();
	//�R�}���h������
	void InitCommand();
	//�X���b�v�`�F�[��������
	void InitSwapChain();
	//�����_�\�^�[�Q�b�g�r���[������
	void InitRTV();
	//�[�x�o�b�t�@������
	void InitDepthBuffer();
	//�t�F���X������
	void InitFence();

public://�ŗL��public�����o�֐�

	//�`�揀��
	void PrepareDraw();
	//�`��㏈��
	void PostDraw();

public: //�Q�b�^�[

	//�f�o�C�X�擾
	ID3D12Device* GetDevice() { return device_.Get(); }
	//�R�}���h���X�g�擾
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

private://�悭�g�������o�ϐ�

	//WindowsAPI
	WinApp* winApp_ = nullptr;

	///�f�o�C�X�֘A
	//DirectX12�̃f�o�C�X
	ComPtr<ID3D12Device> device_ = nullptr;
	//DXGI�t�@�N�g��
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	///�R�}���h�֘A
	//�R�}���h�A���P�[�^
	ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//�R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//�R�}���h�L���[
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	///�X���b�v�`�F�[���֘A
	//�X���b�v�`�F�[��
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

	///�����_�\�^�[�Q�b�g�r���[�֘A
	//�����_�\�^�[�Q�b�g�r���[�q�[�v
	ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
	//�����_�\�^�[�Q�b�g�r���[�f�X�N
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc_{};
	//�o�b�N�o�b�t�@
	std::vector<ComPtr<ID3D12Resource>> backBuffers_;

	//�o���A�f�X�N
	D3D12_RESOURCE_BARRIER barrierDesc_{};

	///�[�x�o�b�t�@�֘A
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff_ = nullptr;
	//�[�x�o�b�t�@�r���[�q�[�v
	ComPtr<ID3D12DescriptorHeap> dsvHeap_ = nullptr;

	///�t�F���X�֘A
	//�t�F���X
	ComPtr<ID3D12Fence> fence_ = nullptr;
	//�t�F���X�l
	UINT64 fenceVal_ = 0;

	///FPS�Œ�
	FixFPS* fixFPS_ = nullptr;

public: //�f�X�g���N�^
	~DirectXBasis();
};