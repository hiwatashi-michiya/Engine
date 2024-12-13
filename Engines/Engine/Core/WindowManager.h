#pragma once
#include <cstdint>
#include <Windows.h>

/// <summary>
/// 画面に表示するウィンドウを生成するクラス
/// </summary>
class WindowManager
{
public:

	static const int kWindowWidth = 1280; //横幅
	static const int kWindowHeight = 720; //縦幅
	//インスタンス取得
	static WindowManager* GetInstance();

	//ウィンドウプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//ウィンドウ生成
	void CreateGameWindow(const wchar_t* windowName = L"MyEngine",
		int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	//メッセージ処理
	bool ProcessMessage();

	//ウィンドウクラスの登録解除
	void TerminateGameWindow();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd() const { return hwnd_; }

	/// <summary>
	/// ウィンドウクラスのインスタンス取得
	/// </summary>
	/// <returns></returns>
	HINSTANCE GetHInstance() const { return wc_.hInstance; }

private:

	//ウィンドウハンドル
	HWND hwnd_ = nullptr;
	//ウィンドウクラス
	WNDCLASS wc_{};

private:
	WindowManager() = default;
	~WindowManager() = default;
	WindowManager(const WindowManager&) = delete;
	const WindowManager& operator=(const WindowManager&) = delete;

};

