#include "Engine.h"
#include <string>
#include "base/SafeDelete.h"
#include <cassert>
#include "Convert.h"
#include "base/TextureManager.h"
#include "input/Input.h"

#ifdef _DEBUG

#include "manager/ImGuiManager.h"

#endif // _DEBUG

WinApp* winApp_ = nullptr;
DirectXCommon* dxCommon_ = nullptr;

void Engine::Initialize(const char* title, int width, int height) {

	CoInitializeEx(0, COINIT_MULTITHREADED);

	//ウィンドウ作成
	auto&& titleString = ConvertString(title);
	winApp_ = WinApp::GetInstance();
	winApp_->CreateGameWindow(
		titleString.c_str(), width, height);

	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->Initialize(winApp_, width, height);

	Sprite::StaticInitialize(dxCommon_->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);
	Model::StaticInitialize(dxCommon_->GetDevice());

	TextureManager::GetInstance()->Initialize();
	Input::GetInstance()->Initialize();

#ifdef _DEBUG

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		TextureManager::GetInstance()->GetSRVDescHeap(),
		TextureManager::GetInstance()->GetSRVDescHeap()->GetCPUDescriptorHandleForHeapStart(),
		TextureManager::GetInstance()->GetSRVDescHeap()->GetGPUDescriptorHandleForHeapStart());

#endif // _DEBUG

}

int Engine::ProcessMessage() {
	return winApp_->ProcessMessage();
}

void Engine::Finalize() {

#ifdef _DEBUG

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#endif // _DEBUG

	Model::Finalize();
	Sprite::Finalize();
	dxCommon_->Finalize();
	dxCommon_ = nullptr;
	CoUninitialize();
	//ウィンドウの破棄
	winApp_->TerminateGameWindow();
}

void Engine::BeginFrame() {

#ifdef _DEBUG

	//フレーム開始を伝える
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

#endif // _DEBUG

	dxCommon_->PreDraw();

	Input::GetInstance()->Update();

}

void Engine::EndFrame() {

#ifdef _DEBUG

	//ImGuiの内部コマンドを生成する
	ImGui::Render();

	//実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());

#endif // _DEBUG

	dxCommon_->PostDraw();

}
