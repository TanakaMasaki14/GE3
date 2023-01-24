#include "ImGuiManager.h"
#include<imgui_impl_win32.h>
#include<imgui_impl_dx12.h>


void ImGuiManager::Initialize()
{
	//imguiのコンテキストを生成
	ImGui::CreateContext();
	//imguiのスタイルを設定
	ImGui::StyleColorsDark();

	//win32用初期化
	ImGui_ImplWin32_Init(WindowsApp::GetInstance().Gethwnd());

	//directx12用初期化
	ImGui_ImplDX12_Init(
		Directx::GetInstance().GetDevice(),
		static_cast<int>(Directx::GetInstance().GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, TextureManager::GetInstance().srvHeap.Get(),
		TextureManager::GetInstance().srvHeap->GetCPUDescriptorHandleForHeapStart(),
		TextureManager::GetInstance().srvHeap->GetGPUDescriptorHandleForHeapStart()
	);

	//標準フォントの追加
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
}

void ImGuiManager::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//デスクリプタヒープを解放(テクスチャクラス作ってそこに移動した方が)
	TextureManager::GetInstance().srvHeap.Reset();
}

void ImGuiManager::Begin()
{
	//imguiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	//描画開始準備(内部の表示物を表示データとしてまとめるので、描画ではない)
	ImGui::Render();
}

void ImGuiManager::Draw()
{
	ID3D12GraphicsCommandList* commandlist = Directx::GetInstance().GetCommandList();

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::GetInstance().srvHeap.Get() };
	commandlist->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandlist);
}
