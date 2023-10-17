#include "Engine.h"
#include <string>
#include "base/SafeDelete.h"
#include <cassert>
#include "Convert.h"

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

}

int Engine::ProcessMessage() {
	return winApp_->ProcessMessage();
}

void Engine::Finalize() {

	Model::Finalize();
	Sprite::Finalize();
	dxCommon_->Finalize();
	dxCommon_ = nullptr;
	CoUninitialize();
	//ウィンドウの破棄
	winApp_->TerminateGameWindow();
}

void Engine::BeginFrame() {

	dxCommon_->PreDraw();

	Sprite::PreDraw(dxCommon_->GetCommandList());

	Model::PreDraw(dxCommon_->GetCommandList());

}

void Engine::EndFrame() {

	Model::PostDraw();

	Sprite::PostDraw();

	dxCommon_->PostDraw();

}
