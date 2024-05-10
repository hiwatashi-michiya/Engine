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

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,1.0f, -0.0f };
	camera_->rotation_.x = 0.0f;

	model_.reset(Model::Create("./Resources/terrain/terrain.obj"));
	transform_ = std::make_unique<Transform>();
	transform_->scale_ = { 10.0f,10.0f,10.0f };

}

void TitleScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("transform");
	ImGui::DragFloat3("scale", &transform_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &transform_->rotate_.x, 0.1f);
	ImGui::DragFloat3("translation", &transform_->translate_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("Manual");
	ImGui::Text("Key S + L_ctrl: Change Scene");
	ImGui::End();

	model_->ImGuiUpdate("terrain");

#endif // _DEBUG

	if (input_->TriggerKey(DIK_S) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("SELECT");
	}

	Quaternion cameraQuaternion = IdentityQuaternion();

	cameraQuaternion = cameraQuaternion.ConvertFromEuler(camera_->rotation_);

	camera_->matRotate_ = MakeRotateMatrix(cameraQuaternion);
	camera_->Update();

	transform_->UpdateMatrix();

	transform_->worldMatrix_.GetRotate();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void TitleScene::DrawModel() {

	model_->Draw(camera_.get());

}

void TitleScene::DrawSkinningModel() {

}

void TitleScene::DrawParticle() {



}

void TitleScene::DrawSprite() {



}

void TitleScene::DrawLine() {



}
