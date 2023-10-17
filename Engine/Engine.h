#pragma once
#include <Windows.h>
#include "base/DirectXCommon.h"
#include "base/WinApp.h"
#include "2d/Sprite.h"
#include "3d/Model.h"

class Engine
{
public:
	
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="title">ウィンドウタイトル</param>
	/// <param name="width">ウィンドウ横幅</param>
	/// <param name="height">ウィンドウ縦幅</param>
	static void Initialize(const char* title, int width, int height);

	/// <summary>
	/// ウィンドウの×ボタンが押されたかどうか
	/// </summary>
	/// <returns></returns>
	static int ProcessMessage();

	/// <summary>
	/// 全体終了
	/// </summary>
	static void Finalize();

	/// <summary>
	/// フレームの開始
	/// </summary>
	static void BeginFrame();

	/// <summary>
	/// フレームの終了
	/// </summary>
	static void EndFrame();

};

struct D3DResourceLeakChecker
{
	~D3DResourceLeakChecker()
	{
		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			/*debug->Release();*/
		}
	}
};