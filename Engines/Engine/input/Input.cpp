#include "Input.h"
#include "Engine/base/WindowManager.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize() {

	WindowManager* winApp = WindowManager::GetInstance();

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

	preJoyState_ = joyState_;

	//キーボード情報の取得開始
	keyboard_->Acquire();

	if (GetJoyState(0, joyState_)) {
		isGetController_ = true;
	}
	else {
		isGetController_ = false;
	}

	CalcDeadZone();

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

bool Input::ReleaseKey(BYTE keyNumber) {

	if (key_[keyNumber] == false && preKey_[keyNumber]) {
		return true;
	}

	return false;

}

Input::~Input() {

	if (keyboard_) {
		keyboard_->Unacquire();
	}

}

bool Input::GetJoyState(int32_t No, XINPUT_STATE& joyState) {

	if (XInputGetState(No, &joyState) == ERROR_SUCCESS) {
		return true;
	}

	return false;

}

bool Input::PushButton(Button button) {

	if (isGetController_) {

		if (joyState_.Gamepad.wButtons & button) {
			return true;
		}

	}

	return false;

}

bool Input::TriggerButton(Button button) {

	if (isGetController_) {

		if ((joyState_.Gamepad.wButtons & button) && (preJoyState_.Gamepad.wButtons & button) == 0) {
			return true;
		}

	}

	return false;

}

bool Input::ReleaseButton(Button button) {

	if (isGetController_) {

		if ((joyState_.Gamepad.wButtons & button) == 0 && (preJoyState_.Gamepad.wButtons & button)) {
			return true;
		}

	}

	return false;

}

void Input::CalcDeadZone() {

	if (fabsf(joyState_.Gamepad.sThumbLX) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
		joyState_.Gamepad.sThumbLX = 0;
	}
	if (fabsf(joyState_.Gamepad.sThumbLY) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
		joyState_.Gamepad.sThumbLY = 0;
	}
	if (fabsf(joyState_.Gamepad.sThumbRX) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
		joyState_.Gamepad.sThumbRX = 0;
	}
	if (fabsf(joyState_.Gamepad.sThumbRY) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
		joyState_.Gamepad.sThumbRY = 0;
	}

}
