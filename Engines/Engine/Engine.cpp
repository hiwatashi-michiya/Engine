#include "Sprite.h"
#include "Model.h"
#include "Particle3D.h"
#include "AudioManager.h"
#include "Core/DirectXSetter.h"
#include "Core/SafeDelete.h"
#include "Core/WindowManager.h"
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
#include "ParticleManager.h"
#include "RenderManager.h"
#include "ModelManager.h"

#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG

#include "ImGuiManager.h"

#endif // _DEBUG

WindowManager* windowManager_ = nullptr;
DirectXSetter* dxSetter_ = nullptr;

using namespace MLEngine;

void Engine::Initialize(const char* title, int width, int height) {

	HRESULT hr;

	hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	assert(SUCCEEDED(hr));

	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	//乱数生成
	SetRandom();

	//ウィンドウ作成
	auto&& titleString = ConvertString(title);
	windowManager_ = WindowManager::GetInstance();
	windowManager_->CreateGameWindow(
		titleString.c_str(), width, height);

	//ここから諸々の初期化処理
	dxSetter_ = DirectXSetter::GetInstance();
	dxSetter_->Initialize(windowManager_, width, height);

	TextureManager::GetInstance()->Initialize(dxSetter_->GetSrvHeap());
	ShaderManager::GetInstance()->Initialize();
	PipelineManager::GetInstance()->Initialize(dxSetter_->GetDevice());
	RootSignatureManager::GetInstance()->Initialize(dxSetter_->GetDevice());

	AudioManager::GetInstance()->Initialize();
	Sprite::StaticInitialize(dxSetter_->GetDevice());
	ModelManager::GetInstance()->Initialize();
	Mesh::StaticInitialize(dxSetter_->GetDevice());
	Material::StaticInitialize(dxSetter_->GetDevice());
	SkinningModel::StaticInitialize(dxSetter_->GetDevice());
	Skybox::Initialize();
	Particle3D::StaticInitialize(dxSetter_->GetDevice());
	Line::Initialize(dxSetter_->GetDevice());
	PostEffectDrawer::GetInstance()->Initialize();

	Input::GetInstance()->Initialize();

	//Engineクラスでインスタンス生成をしておく
	CollisionManager::GetInstance()->Initialize();
	ParticleManager::GetInstance()->Initialize();
	RenderManager::GetInstance()->Clear();

#ifdef _DEBUG

	ImGuiManager::GetInstance()->Initialize();

#endif // _DEBUG

}

int Engine::ProcessMessage() {
	return windowManager_->ProcessMessage();
}

void Engine::Finalize() {

	TextureManager::GetInstance()->Finalize();

#ifdef _DEBUG

	ImGuiManager::GetInstance()->Finalize();

#endif // _DEBUG

	Particle3D::Finalize();
	AudioManager::GetInstance()->Finalize();
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

	ImGuiManager::GetInstance()->BeginFrame();

#endif // _DEBUG

	Input::GetInstance()->Update();

}

void Engine::EndFrame() {

	dxSetter_->Execute();

}
