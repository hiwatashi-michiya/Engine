
#ifdef _DEBUG
#include "ImGuiManager.h"
#include "WindowManager.h"
#include "DirectXSetter.h"
#include "TextureManager.h"

ImGuiManager* ImGuiManager::GetInstance()
{
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize()
{

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WindowManager::GetInstance()->GetHwnd());
	ImGui_ImplDX12_Init(DirectXSetter::GetInstance()->GetDevice(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		TextureManager::GetInstance()->GetSRVDescHeap(),
		TextureManager::GetInstance()->GetSRVDescHeap()->GetCPUDescriptorHandleForHeapStart(),
		TextureManager::GetInstance()->GetSRVDescHeap()->GetGPUDescriptorHandleForHeapStart());

}

void ImGuiManager::Finalize()
{

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void ImGuiManager::BeginFrame()
{

	//フレーム開始を伝える
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

}

void ImGuiManager::Render()
{

	//ImGuiの内部コマンドを生成する
	ImGui::Render();

	//実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXSetter::GetInstance()->GetCommandList());

}



#endif // _DEBUG