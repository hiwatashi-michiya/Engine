#include "SelectScene.h"
#include "FrameWork/SceneManager.h"
#include "PostEffectDrawer.h"
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

	ImGui::Begin("Frame Late");
	ImGui::Text("%1.2f", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Scene Change");
	ImGui::Text("Key 1 or 3 + L_ctrl: Change Scene\n1 : title\n3 : game");
	ImGui::End();

	if (input_->TriggerKey(DIK_1) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else if (input_->TriggerKey(DIK_3) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	else if (input_->TriggerKey(DIK_4) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("EDITOR");
	}

#endif // _DEBUG

	if (input_->TriggerButton(Input::Button::LEFT) || input_->TriggerLStick(Input::StickArrow::S_LEFT)) {

		if (stageNumber_ > 1) {
			stageNumber_--;
		}

	}

	if (input_->TriggerButton(Input::Button::RIGHT) || input_->TriggerLStick(Input::StickArrow::S_RIGHT)) {

		if (stageNumber_ < kMaxStage_) {
			stageNumber_++;
		}

	}

	if (input_->TriggerButton(Input::Button::A) && !SceneChangeManager::GetInstance()->IsSceneChange()) {
		SceneChangeManager::GetInstance()->SetNextScene("GAMEPLAY");
		SceneChangeManager::GetInstance()->SceneChangeStart();
	}

	if (input_->TriggerButton(Input::Button::B) && !SceneChangeManager::GetInstance()->IsSceneChange()) {
		SceneChangeManager::GetInstance()->SetNextScene("TITLE");
		SceneChangeManager::GetInstance()->SceneChangeStart();
	}

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();


}

void SelectScene::DrawModel() {

	
}

void SelectScene::DrawSkinningModel() {



}

void SelectScene::DrawParticle() {



}

void SelectScene::DrawSprite() {



}

void SelectScene::DrawLine() {

}
