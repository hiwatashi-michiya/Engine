#include "Input.h"
#include "Engine/base/WinApp.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize() {

	WinApp* winApp = WinApp::GetInstance();

	hwnd_ = winApp->GetHwnd();

	HRESULT hr = S_FALSE;

	//DirectInputの初期化
	hr = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&dInput_, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの生成
	hr = dInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));

	//入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard); //標準形式
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(
		hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

}

void Input::Update() {

	//キーボード情報の取得開始
	keyboard_->Acquire();

	preKey_ = key_;

	key_.fill(0);
	keyboard_->GetDeviceState(sizeof(key_), key_.data());

}

bool Input::PushKey(BYTE keyNumber) {

	if (key_[keyNumber]) {
		return true;
	}

	return false;

}

bool Input::TriggerKey(BYTE keyNumber) {

	if (key_[keyNumber] && preKey_[keyNumber] == false) {
		return true;
	}

	return false;

}

Input::~Input() {

	if (keyboard_) {
		keyboard_->Unacquire();
	}

}
