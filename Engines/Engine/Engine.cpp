#include "Engine.h"
#include <Windows.h>
#include "base/DirectXCommon.h"
#include "base/WinApp.h"
#include "2d/Sprite.h"
#include "3d/Model.h"
#include <string>
#include "base/SafeDelete.h"
#include <cassert>
#include "Convert.h"
#include "manager/TextureManager.h"
#include "input/Input.h"
#include "3d/Particle3D.h"
#include "manager/AudioManager.h"
#include "math/Rand.h"
#include "Tool/GlobalVariables.h"

#ifdef _DEBUG

#include "manager/ImGuiManager.h"

#endif // _DEBUG

#include "manager/ImGuiManager.h"

WinApp* winApp_ = nullptr;
DirectXCommon* dxCommon_ = nullptr;

void Engine::Initialize(const char* title, int width, int height) {

	CoInitializeEx(0, COINIT_MULTITHREADED);

	//乱数生成
	SetRandom();

	//ウィンドウ作成
	auto&& titleString = ConvertString(title);
	winApp_ = WinApp::GetInstance();
	winApp_->CreateGameWindow(
		titleString.c_str(), width, height);

	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->Initialize(winApp_, width, height);

	AudioManager::GetInstance()->Initialize();
	Sprite::StaticInitialize(dxCommon_->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);
	Model::StaticInitialize(dxCommon_->GetDevice());
	Particle3D::StaticInitialize(dxCommon_->GetDevice());

	
	TextureManager::GetInstance()->Initialize();
	Input::GetInstance()->Initialize();
	GlobalVariables::GetInstance()->LoadFiles();

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

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Particle3D::Finalize();
	AudioManager::GetInstance()->Finalize();
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

	//フレーム開始を伝える
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	dxCommon_->PreDraw();

	Input::GetInstance()->Update();

	Model::PreDraw(dxCommon_->GetCommandList());
	Particle3D::PreDraw(dxCommon_->GetCommandList());
	Sprite::PreDraw(dxCommon_->GetCommandList());

	GlobalVariables::GetInstance()->Update();

}

void Engine::EndFrame() {

	Model::PostDraw();
	Particle3D::PostDraw();
	Sprite::PostDraw();

#ifdef _DEBUG

	//ImGuiの内部コマンドを生成する
	ImGui::Render();

	//実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());

#endif // _DEBUG

	//ImGuiの内部コマンドを生成する
	ImGui::Render();

	//実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());

	dxCommon_->PostDraw();

}
