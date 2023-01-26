#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include<wrl.h>
#include"WindowsAPI.h"
#include<vector>
#include<chrono>



class ReDirectX
{
public:
	//�e�평�����p�ϐ�
	HRESULT result;

	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff;
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
private:
	//WIndowsAPI
	WindowsAPI* windowsAPI = nullptr;

	//�L�^����(FPS�Œ�p)
	std::chrono::steady_clock::time_point reference_;
public:
	//������
	void Initialize(WindowsAPI* windowsAPI);

	//�`��O����(���t���[������)
	void BeginDraw();

	//�`��㏈��(���t���[������)
	void EndDraw();

	//�f�o�C�X�擾
	ID3D12Device* GetDevice() { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList.Get(); }

	size_t GetBackBufferCount()const { return backBuffers.size(); }
private:

	//�e�평�����֐�
	void InitializeDevice();
	void InitializeCommand();
	void InitializeSwapChain();
	void InitializeRTV();
	void InitializeDepthBuff();
	void InitializeFence();

	//FPS������
	void InitializeFixFPS();
	//FPS�X�V
	void UpdateFixFPS();

};

