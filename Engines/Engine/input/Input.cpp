#include "Input.h"
#include "Core/WindowManager.h"
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

	//マウスデバイスの生成
	hr = dInput_->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(hr));

	//入力データ形式のセット
	hr = mouseDevice_->SetDataFormat(&c_dfDIMouse2); //拡張8ボタン
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(
		hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr));

}

void Input::Update() {

	preJoyState_ = joyState_;

	//キーボード情報の取得開始
	keyboard_->Acquire();
	//マウスの取得開始
	mouseDevice_->Acquire();

	if (GetJoyState(0, joyState_)) {
		isGetController_ = true;
	}
	else {
		isGetController_ = false;
	}

	CalcDeadZone();

	//前回入力の保存
	preKey_ = key_;
	preMouse_ = mouse_;

	//キー入力
	key_.fill(0);
	keyboard_->GetDeviceState(sizeof(key_), key_.data());

	//マウス入力
	std::memset(&mouse_, 0, sizeof(mouse_));
	mouseDevice_->GetDeviceState(sizeof(mouse_), &mouse_);

	//スクリーン座標の取得
	POINT mousePosition;
	GetCursorPos(&mousePosition);

	//ウィンドウ座標へ変換
	ScreenToClient(hwnd_, &mousePosition);
	mousePosition_.x = float(mousePosition.x);
	mousePosition_.y = float(mousePosition.y);

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

bool Input::PushMouse(Mouse mouse) {

	return (mouse_.rgbButtons[mouse] & 0x80) != 0;

}

bool Input::TriggerMouse(Mouse mouse) {

	return (mouse_.rgbButtons[mouse] & 0x80) != 0 &&
		(preMouse_.rgbButtons[mouse] & 0x80) == 0;

}

bool Input::ReleaseMouse(Mouse mouse) {

	return (mouse_.rgbButtons[mouse] & 0x80) == 0 &&
		(preMouse_.rgbButtons[mouse] & 0x80) != 0;

}

Vector2 Input::GetMouseMove() {
	Vector2 tmp{};
	tmp.x = float(mouse_.lX);
	tmp.y = float(mouse_.lY);
	return tmp;
}

Input::~Input() {

	if (keyboard_) {
		keyboard_->Unacquire();
	}
	if (mouseDevice_) {
		mouseDevice_->Unacquire();
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

bool Input::TriggerLStick(StickArrow arrow) {

	switch (arrow)
	{
	case Input::S_UP:
		return joyState_.Gamepad.sThumbLY > 0 && !preJoyState_.Gamepad.sThumbLY;
		break;
	case Input::S_DOWN:
		return joyState_.Gamepad.sThumbLY < 0 && !preJoyState_.Gamepad.sThumbLY;
		break;
	case Input::S_LEFT:
		return joyState_.Gamepad.sThumbLX < 0 && !preJoyState_.Gamepad.sThumbLX;
		break;
	case Input::S_RIGHT:
		return joyState_.Gamepad.sThumbLX > 0 && !preJoyState_.Gamepad.sThumbLX;
		break;
	default:
		return false;
		break;
	}

	return false;

}

bool Input::TriggerRStick(StickArrow arrow) {

	switch (arrow)
	{
	case Input::S_UP:
		return joyState_.Gamepad.sThumbRY > 0 && !preJoyState_.Gamepad.sThumbRY;
		break;
	case Input::S_DOWN:
		return joyState_.Gamepad.sThumbRY < 0 && !preJoyState_.Gamepad.sThumbRY;
		break;
	case Input::S_LEFT:
		return joyState_.Gamepad.sThumbRX < 0 && !preJoyState_.Gamepad.sThumbRX;
		break;
	case Input::S_RIGHT:
		return joyState_.Gamepad.sThumbRX > 0 && !preJoyState_.Gamepad.sThumbRX;
		break;
	default:
		return false;
		break;
	}

	return false;

}

SHORT Input::GetStickValue(Stick stick) {

	//いずれかのスティックの値を返す
	switch (stick)
	{
	case Input::LX:
		return joyState_.Gamepad.sThumbLX;
		break;
	case Input::LY:
		return joyState_.Gamepad.sThumbLY;
		break;
	case Input::RX:
		return joyState_.Gamepad.sThumbRX;
		break;
	case Input::RY:
		return joyState_.Gamepad.sThumbRY;
		break;
	default:
		return 0;
		break;
	}

	return 0;

}
