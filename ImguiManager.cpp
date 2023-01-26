#include "ImguiManager.h"

void ImguiManager::Initialize(WindowsAPI* winApp, ReDirectX* directX_)
{
	HRESULT result;

	assert(directX_);
	directX = directX_;

	//imguiセットアップ
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	//ImGui用でスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC desc{};

	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = directX->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descHeap));

	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(directX->GetDevice(), directX->GetBackBufferCount(),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, descHeap.Get(),
		descHeap.Get()->GetCPUDescriptorHandleForHeapStart(),
		descHeap.Get()->GetGPUDescriptorHandleForHeapStart());
}

void ImguiManager::Begin()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImguiManager::End()
{
	ImGui::Render();
}

void ImguiManager::Draw()
{
	ID3D12GraphicsCommandList* cmdList = directX->GetCommandList();
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	directX->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
}

void ImguiManager::Finalize()
{
	//ImGui終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
