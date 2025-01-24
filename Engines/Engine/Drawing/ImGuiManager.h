#pragma once

#ifdef _DEBUG
//ImGUiのインクルード一括纏め
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>

/// <summary>
/// ImGuiに関する処理を管理するクラス
/// </summary>
class ImGuiManager
{
public:
	
	//インスタンス取得
	static ImGuiManager* GetInstance();
	//初期化
	void Initialize();
	//終了処理
	void Finalize();
	//フレーム開始処理
	void BeginFrame();
	//描画コマンド積む
	void Render();

private:
	//シングルトン化
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;

};

#endif // _DEBUG
