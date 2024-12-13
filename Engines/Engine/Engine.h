#pragma once

/// <summary>
/// ゲームエンジン
/// </summary>
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

