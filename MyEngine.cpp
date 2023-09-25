#include "MyEngine.h"
#include <string>
#include "SafeDelete.h"
#include <cassert>

std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0,
		reinterpret_cast<const char*>(&str[0]), static_cast<int>
		(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const
		char*>(&str[0]), static_cast<int>(str.size()), &result[0],
		sizeNeeded);
	return result;

}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0,
		str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>
		(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;

}

namespace {

	class MyEngineSys
	{
		friend class MyEngine;
	public:
		MyEngineSys() = default;
		~MyEngineSys() = default;

		void Initialize();

	private:

		int ProcessMessage();

		void BeginFrame();
		void EndFrame();

		WinApp* winApp_ = nullptr;
		DirectXCommon* dxCommon_ = nullptr;

	};

	WinApp* sWinApp = nullptr;
	DirectXCommon* sDxCommon = nullptr;
	MyEngineSys* sMyEngineSys = nullptr;
}

void MyEngineSys::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	winApp_ = WinApp::GetInstance();

}

void MyEngineSys::BeginFrame() {
	
	dxCommon_->PreDraw();

}

void MyEngineSys::EndFrame() {

	dxCommon_->PostDraw();

}

int MyEngineSys::ProcessMessage() {
	return winApp_->ProcessMessage();
}

void MyEngine::Initialize(const char* title, int width, int height) {

	assert(!sWinApp);
	assert(!sDxCommon);

	//ウィンドウ作成
	auto&& titleString = ConvertString(title);
	sWinApp = WinApp::GetInstance();
	sWinApp->CreateGameWindow(
		titleString.c_str(), width, height);

	sDxCommon = DirectXCommon::GetInstance();
	sDxCommon->Initialize(sWinApp, width, height);

	sMyEngineSys = new MyEngineSys();
	sMyEngineSys->Initialize();

}

int MyEngine::ProcessMessage() {
	return sMyEngineSys->ProcessMessage();
}

void MyEngine::Finalize() {
	//解放
	SafeDelete(sMyEngineSys);
	//ウィンドウの破棄
	sWinApp->TerminateGameWindow();
}

void MyEngine::BeginFrame() {

	sMyEngineSys->BeginFrame();

}

void MyEngine::EndFrame() {

	sMyEngineSys->EndFrame();

}
