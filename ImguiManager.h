#pragma once
#include<imgui.h>
#include<imgui_impl_win32.h>
#include<imgui_impl_dx12.h>
#include<dxgi1_4.h>
#include"WindowsAPI.h"
#include"DirectX.h"
#include<wrl.h>

class ImguiManager
{
public:

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;

public:

	ReDirectX* directX = nullptr;

	void Initialize(WindowsAPI* winApp, ReDirectX* directX_);

	void Begin();

	void End();

	void Draw();

	void Finalize();
};

