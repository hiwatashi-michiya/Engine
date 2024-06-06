#include "SelectScene.h"
#include "FrameWork/SceneManager.h"
#include <cmath>

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

SelectScene::SelectScene()
{

}

SelectScene::~SelectScene()
{
}

void SelectScene::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();
	audioManager_ = AudioManager::GetInstance();

	editor_ = MapEditor::GetInstance();
	editor_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,2.0f, -20.0f };
	camera_->rotation_.y = 0.0f;

	editor_->SetCamera(camera_.get());

}

void SelectScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	Vector3 moves{};

	moves.x = input_->GetMouseMove().x;
	moves.y = input_->GetMouseMove().y;
	moves.z = float(input_->GetMouseWheel());

	ImGui::Begin("mouse");
	ImGui::Text("Pos x : %1.2f,  y : %1.2f", input_->GetMousePosition().x, input_->GetMousePosition().y);
	ImGui::Text("move & wheel move x : %1.2f, y : %1.2f, wheel : %1.2f", moves.x, moves.y, moves.z);

	if (input_->PushMouse(Input::kLeft)) {
		ImGui::Text("Left Push");
	}
	if (input_->PushMouse(Input::kRight)) {
		ImGui::Text("Right Push");
	}
	if (input_->PushMouse(Input::kCenter)) {
		ImGui::Text("Center Push");
	}

	if (input_->TriggerMouse(Input::kLeft) || input_->TriggerMouse(Input::kRight)
		|| input_->TriggerMouse(Input::kCenter)) {
		ImGui::Text("Trigger");
	}

	if (input_->ReleaseMouse(Input::kLeft) || input_->ReleaseMouse(Input::kRight)
		|| input_->ReleaseMouse(Input::kCenter)) {
		ImGui::Text("Release");
	}

	ImGui::End();

#endif // _DEBUG


	editor_->Edit();

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();


}

void SelectScene::DrawModel() {

	editor_->Draw(camera_.get());

}

void SelectScene::DrawSkinningModel() {

}

void SelectScene::DrawParticle() {



}

void SelectScene::DrawSprite() {



}

void SelectScene::DrawLine() {

}
