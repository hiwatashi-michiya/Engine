#include "WindowManager.h"

#ifdef _DEBUG

#include "externals/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // _DEBUG

WindowManager* WindowManager::GetInstance() {
	static WindowManager instance;
	return &instance;
}

//ウィンドウプロシージャ
LRESULT WindowManager::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

#ifdef _DEBUG

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

#endif // _DEBUG

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{

		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);

}

void WindowManager::CreateGameWindow(const wchar_t* windowName, int32_t clientWidth, int32_t clientHeight) {

	

	//ウィンドウプロシージャ
	wc_.lpfnWndProc = (WNDPROC)WindowProc;

	//ウィンドウクラス名(なんでも良い)
	wc_.lpszClassName = windowName;

	//インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);

	//カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスを登録する
	RegisterClass(&wc_);

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0, clientWidth, clientHeight };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウの生成
	hwnd_ = CreateWindow(
		wc_.lpszClassName,      //利用するクラス名
		windowName,                //タイトルバーの文字(何でも良い)
		WS_OVERLAPPEDWINDOW,   //よく見るウィンドウスタイル
		CW_USEDEFAULT,         //表示X座標(Windowsに任せる)
		CW_USEDEFAULT,         //表示Y座標(WindowsOSに任せる)
		wrc.right - wrc.left,  //ウィンドウ横幅
		wrc.bottom - wrc.top,  //ウィンドウ縦幅
		nullptr,               //横ウィンドウハンドル
		nullptr,               //メニューハンドル
		wc_.hInstance,          //インスタンスハンドル
		nullptr);              //オプション

	//ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);

}

bool WindowManager::ProcessMessage() {

	//メッセージ
	MSG msg{};

	//メッセージがあるか確認
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg); //キー入力メッセージの処理
		DispatchMessage(&msg); //ウィンドウプロシージャにメッセージを送る
	}

	//終了メッセージが来たらループを抜ける
	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;

}

void WindowManager::TerminateGameWindow() {

	UnregisterClass(wc_.lpszClassName, wc_.hInstance);

}
