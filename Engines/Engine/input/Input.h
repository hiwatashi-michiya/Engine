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
	//スティック
	enum Stick {
		LX,
		LY,
		RX,
		RY,
	};
	//スティックの上下左右判定
	enum StickArrow {
		S_UP,
		S_DOWN,
		S_LEFT,
		S_RIGHT,
	};
	//マウスボタン
	enum Mouse {
		kLeft,
		kRight,
		kCenter,
	};
	//インスタンス取得
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
	//キーが押された瞬間
	bool TriggerKey(BYTE keyNumber);
	//キーが離された瞬間
	bool ReleaseKey(BYTE keyNumber);
	//コントローラ起動フラグ取得
	bool GetJoyState(int32_t No, XINPUT_STATE& joyState);
	//ボタンが押されているか
	bool PushButton(Button button);
	//ボタンを押した瞬間
	bool TriggerButton(Button button);
	//ボタンを離した瞬間
	bool ReleaseButton(Button button);
	//マウスボタンを押しているか
	bool PushMouse(Mouse mouse);
	//マウスボタンを押した瞬間
	bool TriggerMouse(Mouse mouse);
	//マウスボタンを離した瞬間
	bool ReleaseMouse(Mouse mouse);
	//いずれかの方向に左スティックを倒した瞬間
	bool TriggerLStick(StickArrow arrow);
	//いずれかの方向に右スティックを倒した瞬間
	bool TriggerRStick(StickArrow arrow);
	//マウスの移動取得
	Vector2 GetMouseMove();
	//マウスホイールの移動量取得
	int32_t GetMouseWheel() const { return mouse_.lZ; }
	//ゲームパッド取得
	XINPUT_GAMEPAD GetGamepad() { return joyState_.Gamepad; }

	//コントローラーが繋がれているか
	bool GetIsGamepad() { return isGetController_; }

	//スティックのそれぞれの値を取得
	SHORT GetStickValue(Stick stick);
	//マウスの座標取得
	Vector2 GetMousePosition() const { return mousePosition_; }

private:

	//シングルトン化
	Input() = default;
	~Input();
	Input(const Input&) = delete;
	const Input& operator=(const Input&) = delete;

private:
	//デッドゾーン計算
	void CalcDeadZone();

	Microsoft::WRL::ComPtr<IDirectInput8> dInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouseDevice_ = nullptr;

	//コントローラの現在と一フレーム前の状態
	XINPUT_STATE joyState_;
	XINPUT_STATE preJoyState_;

	//キーの現在と一フレーム前の状態
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> preKey_;
	//マウスの現在と一フレーム前の状態
	DIMOUSESTATE2 mouse_;
	DIMOUSESTATE2 preMouse_;
	HWND hwnd_;
	//マウス座標
	Vector2 mousePosition_{};
	//コントローラに繋がれているか
	bool isGetController_ = false;

};

