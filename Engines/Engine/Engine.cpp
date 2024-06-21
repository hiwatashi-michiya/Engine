#include "Sprite.h"
#include "Model.h"
#include "Particle3D.h"
#include "AudioManager.h"
#include "DirectXSetter.h"
#include "SafeDelete.h"
#include "WindowManager.h"
#include "Convert.h"
#include "LineDrawer.h"
#include "PipelineManager.h"
#include "PostEffectDrawer.h"
#include "RootSignatureManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Engine.h"
#include "Input.h"
#include "Rand.h"
#include "Skinning/SkinningModel.h"
#include "Tool/GlobalVariables.h"
#include <cassert>
#include <string>
#include <Windows.h>
#include "CollisionManager.h"
#include "Skybox.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

WindowManager* windowManager_ = nullptr;
DirectXSetter* dxSetter_ = nullptr;

void Engine::Initialize(const char* title, int width, int height) {

	CoInitializeEx(0, COINIT_MULTITHREADED);

	//乱数生成
	SetRandom();

	//ウィンドウ作成
	auto&& titleString = ConvertString(title);
	windowManager_ = WindowManager::GetInstance();
	windowManager_->CreateGameWindow(
		titleString.c_str(), width, height);

	dxSetter_ = DirectXSetter::GetInstance();
	dxSetter_->Initialize(windowManager_, width, height);

	ShaderManager::GetInstance()->Initialize();
	PipelineManager::GetInstance()->Initialize(dxSetter_->GetDevice());
	RootSignatureManager::GetInstance()->Initialize(dxSetter_->GetDevice());

	AudioManager::GetInstance()->Initialize();
	Sprite::StaticInitialize(dxSetter_->GetDevice(), WindowManager::kWindowWidth, WindowManager::kWindowHeight);
	Model::StaticInitialize(dxSetter_->GetDevice());
	SkinningModel::StaticInitialize(dxSetter_->GetDevice());
	Skybox::Initialize();
	Particle3D::StaticInitialize(dxSetter_->GetDevice());
	Line::Initialize(dxSetter_->GetDevice());
	PostEffectDrawer::GetInstance()->Initialize();

	TextureManager::GetInstance()->Initialize(dxSetter_->GetSrvHeap());
	Input::GetInstance()->Initialize();

	CollisionManager::GetInstance()->Initialize();

#ifdef _DEBUG

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowManager_->GetHwnd());
	ImGui_ImplDX12_Init(dxSetter_->GetDevice(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		TextureManager::GetInstance()->GetSRVDescHeap(),
		TextureManager::GetInstance()->GetSRVDescHeap()->GetCPUDescriptorHandleForHeapStart(),
		TextureManager::GetInstance()->GetSRVDescHeap()->GetGPUDescriptorHandleForHeapStart());

#endif // _DEBUG

}

int Engine::ProcessMessage() {
	return windowManager_->ProcessMessage();
}

void Engine::Finalize() {

	TextureManager::GetInstance()->Finalize();

#ifdef _DEBUG

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#endif // _DEBUG

	Particle3D::Finalize();
	AudioManager::GetInstance()->Finalize();
	Model::Finalize();
	SkinningModel::Finalize();
	Sprite::Finalize();
	dxSetter_->Finalize();
	dxSetter_ = nullptr;
	CoUninitialize();
	//ウィンドウの破棄
	windowManager_->TerminateGameWindow();
}

void Engine::BeginFrame() {

#ifdef _DEBUG

	//フレーム開始を伝える
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

#endif // _DEBUG

	Input::GetInstance()->Update();

}

void Engine::EndFrame() {

#ifdef _DEBUG

	//ImGuiの内部コマンドを生成する
	ImGui::Render();

	//実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxSetter_->GetCommandList());

#endif // _DEBUG

	dxSetter_->PostDraw();

}
