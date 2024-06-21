#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <array>
#include <dinput.h>
#include <wrl.h>
#include <Xinput.h>
#include "Vector2.h"

class Input
{
public:

	//押し込みボタン
	enum Button {
		A = XINPUT_GAMEPAD_A,
		B = XINPUT_GAMEPAD_B,
		X = XINPUT_GAMEPAD_X,
		Y = XINPUT_GAMEPAD_Y,
		UP = XINPUT_GAMEPAD_DPAD_UP,
		DOWN = XINPUT_GAMEPAD_DPAD_DOWN,
		RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,
		LEFT = XINPUT_GAMEPAD_DPAD_LEFT,
		RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,
		LB = XINPUT_GAMEPAD_LEFT_SHOULDER,
		RThumb = XINPUT_GAMEPAD_RIGHT_THUMB,
		LThumb = XINPUT_GAMEPAD_LEFT_THUMB,
		START = XINPUT_GAMEPAD_START,
		BACK = XINPUT_GAMEPAD_BACK,
	};

	enum Stick {
		LX,
		LY,
		RX,
		RY,
	};

	enum Mouse {
		kLeft,
		kRight,
		kCenter,
	};

	static Input* GetInstance();

	//初期化
	void Initialize();

	//更新
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber"></param>
	/// <returns></returns>
	bool PushKey(BYTE keyNumber);

	bool TriggerKey(BYTE keyNumber);

	bool ReleaseKey(BYTE keyNumber);

	bool GetJoyState(int32_t No, XINPUT_STATE& joyState);

	bool PushButton(Button button);

	bool TriggerButton(Button button);

	bool ReleaseButton(Button button);

	bool PushMouse(Mouse mouse);

	bool TriggerMouse(Mouse mouse);

	bool ReleaseMouse(Mouse mouse);

	Vector2 GetMouseMove();

	int32_t GetMouseWheel() const { return mouse_.lZ; }

	XINPUT_GAMEPAD GetGamepad() { return joyState_.Gamepad; }

	//コントローラーが繋がれているか
	bool GetIsGamepad() { return isGetController_; }

	//スティックのそれぞれの値を取得
	SHORT GetStickValue(Stick stick);

	Vector2 GetMousePosition() const { return mousePosition_; }

private:

	Input() = default;
	~Input();
	Input(const Input&) = delete;
	const Input& operator=(const Input&) = delete;

private:

	void CalcDeadZone();

	Microsoft::WRL::ComPtr<IDirectInput8> dInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouseDevice_ = nullptr;

	XINPUT_STATE joyState_;
	XINPUT_STATE preJoyState_;

	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> preKey_;
	DIMOUSESTATE2 mouse_;
	DIMOUSESTATE2 preMouse_;
	HWND hwnd_;
	bool isGetController_ = false;

	Vector2 mousePosition_{};

};

