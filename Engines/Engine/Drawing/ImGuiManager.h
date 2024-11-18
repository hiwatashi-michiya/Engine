#pragma once

#ifdef _DEBUG
//ImGUiのインクルード一括纏め
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>

class ImGuiManager
{
public:
	


private:

	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;

};

#endif // _DEBUG
