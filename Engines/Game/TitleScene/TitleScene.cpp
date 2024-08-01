#include "TitleScene.h"
#include "FrameWork/SceneManager.h"
#include "UsefulFunc.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

TitleScene::TitleScene()
{
	aTex_ = TextureManager::GetInstance()->Load("./Resources/UI/A_start.png");
	titleTex_ = TextureManager::GetInstance()->Load("./Resources/UI/title.png");
	skyDome_.reset(Model::Create("./Resources/skydome/temp.obj"));
	title_.reset(Sprite::Create(titleTex_, { 640.0f - 512.0f,160.0f - 128.0f }));
	aButton_.reset(Sprite::Create(aTex_, { 640.0f - 128.0f,560.0f - 32.0f }));
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

	skyDomeTransform_ = std::make_unique<Transform>();
	skyDomeTransform_->scale_ = { 500.0f,500.0f,500.0f };
	skyDome_->material_->dLightMap_->direction = { 0.0f,-0.8f,0.6f };
	skyDome_->material_->dLightMap_->intensity = 1.0f;
	skyDome_->material_->pLightMap_->radius = 650.0f;
	skyDome_->material_->pLightMap_->intensity = 0.15f;
	skyDome_->material_->pLightMap_->decay = 0.5f;
	skyDomeTransform_->UpdateMatrix();
	skyDome_->SetWorldMatrix(skyDomeTransform_->worldMatrix_);

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

	skyDome_->Draw(camera_.get());

}

void TitleScene::DrawSkinningModel() {

}

void TitleScene::DrawParticle() {



}

void TitleScene::DrawSprite() {

	title_->Draw();
	aButton_->Draw();

}

void TitleScene::DrawLine() {



}
