#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <wrl.h>
#include <dinput.h>
#include <array>

class Input
{
public:

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

private:

	Input() = default;
	~Input();
	Input(const Input&) = delete;
	const Input& operator=(const Input&) = delete;

private:

	Microsoft::WRL::ComPtr<IDirectInput8> dInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> preKey_;
	HWND hwnd_;

};

