#include "EditorScene.h"
#include "FrameWork/SceneManager.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

EditorScene::EditorScene()
{

}

EditorScene::~EditorScene()
{
}

void EditorScene::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();
	editor_ = MapEditor::GetInstance();
	editor_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,10.0f, -30.0f };
	camera_->rotation_.x = 0.3f;


}

void EditorScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("Manual");
	ImGui::Text("Key S: Change Scene");
	ImGui::End();

	editor_->Edit();

#endif // _DEBUG

	if (input_->TriggerKey(DIK_S)) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

}

void EditorScene::Draw() {

	editor_->Draw(camera_.get());

}
