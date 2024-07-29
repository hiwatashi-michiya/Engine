#include "TitleScene.h"
#include "FrameWork/SceneManager.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize() {

	editor_ = MapEditor::GetInstance();
	editor_->Initialize();

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,65.0f, -60.0f };
	camera_->rotation_.x = 0.9f;

	editor_->SetCamera(camera_.get());

}

void TitleScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("Scene Change");
	ImGui::Text("Key 2 or 3 + L_ctrl: Change Scene\n2 : select\n3 : game");
	ImGui::End();

	if (input_->TriggerKey(DIK_2) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("SELECT");
	}
	else if (input_->TriggerKey(DIK_3) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	else if (input_->TriggerKey(DIK_4) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("EDITOR");
	}

#endif // _DEBUG

	if (input_->TriggerButton(Input::Button::A) && !SceneChangeManager::GetInstance()->IsSceneChange()) {
		SceneChangeManager::GetInstance()->SetNextScene("SELECT");
		SceneChangeManager::GetInstance()->SceneChangeStart();
	}

	Quaternion cameraQuaternion = IdentityQuaternion();

	cameraQuaternion = cameraQuaternion.ConvertFromEuler(camera_->rotation_);

	camera_->matRotate_ = MakeRotateMatrix(cameraQuaternion);
	camera_->Update();
	
}

void TitleScene::DrawModel() {

	

}

void TitleScene::DrawSkinningModel() {

}

void TitleScene::DrawParticle() {



}

void TitleScene::DrawSprite() {



}

void TitleScene::DrawLine() {



}
