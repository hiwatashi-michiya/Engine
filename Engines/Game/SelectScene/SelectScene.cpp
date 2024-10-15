#include "SelectScene.h"
#include "FrameWork/SceneManager.h"
#include "PostEffectDrawer.h"
#include <cmath>
#include "UsefulFunc.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

SelectScene::SelectScene()
{
	stageSelectTex_ = TextureManager::GetInstance()->Load("./Resources/UI/stageSelect.png");
	toTitleTex_ = TextureManager::GetInstance()->Load("./Resources/UI/toTitle.png");
	Tex1_ = TextureManager::GetInstance()->Load("./Resources/UI/1.png");
	Tex2_ = TextureManager::GetInstance()->Load("./Resources/UI/2.png");
	Tex3_ = TextureManager::GetInstance()->Load("./Resources/UI/3.png");
	numTex_ = TextureManager::GetInstance()->Load("./Resources/UI/num.png");
	buttonTex_ = TextureManager::GetInstance()->Load("./Resources/UI/a.png");
	skyDome_.reset(Model::Create("./Resources/skydome/temp.obj"));
	stageSelect_.reset(Sprite::Create(stageSelectTex_, { 640.0f - 256.0f,100.0f - 32.0f }));
	toTitle_.reset(Sprite::Create(toTitleTex_, { 140.0f - 128.0f,60.0f - 32.0f }));
	stage1_.reset(Sprite::Create(Tex1_, { 440.0f - 32.0f,360.0f - 32.0f }));
	stage2_.reset(Sprite::Create(Tex2_, { 640.0f - 32.0f,360.0f - 32.0f }));
	stage3_.reset(Sprite::Create(Tex3_, { 840.0f - 32.0f,360.0f - 32.0f }));
	num_.reset(Sprite::Create(numTex_, { 640.0f - 32.0f,360.0f - 32.0f }));
	num_->size_ = { 64.0f,64.0f };
	num_->viewRect_ = { 0.1f,1.0f };
	aButton_.reset(Sprite::Create(buttonTex_, { 640.0f - 32.0f,600.0f - 32.0f }));

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

	if (input_->TriggerKey(DIK_1) and input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else if (input_->TriggerKey(DIK_3) and input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	else if (input_->TriggerKey(DIK_4) and input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("EDITOR");
	}

	if (input_->TriggerKey(DIK_LEFT) || input_->TriggerKey(DIK_A)) {

		if (stageNumber_ > 1) {
			stageNumber_--;
		}

	}

	if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(DIK_D)) {

		if (stageNumber_ < kMaxStage_) {
			stageNumber_++;
		}

	}

	if (input_->TriggerKey(DIK_SPACE) and !SceneChangeManager::GetInstance()->IsSceneChange()) {
		SceneChangeManager::GetInstance()->SetNextScene("GAMEPLAY");
		SceneChangeManager::GetInstance()->SceneChangeStart();
	}

	if (input_->TriggerKey(DIK_Q) and !SceneChangeManager::GetInstance()->IsSceneChange()) {
		SceneChangeManager::GetInstance()->SetNextScene("TITLE");
		SceneChangeManager::GetInstance()->SceneChangeStart();
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

	if (input_->TriggerButton(Input::Button::A) and !SceneChangeManager::GetInstance()->IsSceneChange()) {
		SceneChangeManager::GetInstance()->SetNextScene("GAMEPLAY");
		SceneChangeManager::GetInstance()->SceneChangeStart();
	}

	if (input_->TriggerButton(Input::Button::B) and !SceneChangeManager::GetInstance()->IsSceneChange()) {
		SceneChangeManager::GetInstance()->SetNextScene("TITLE");
		SceneChangeManager::GetInstance()->SceneChangeStart();
	}

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

	if (stageNumber_ == 1) {
		stage1_->position_ = { 440.0f - 32.0f,310.0f - 32.0f };
		stage2_->position_ = { 640.0f - 32.0f,360.0f - 32.0f };
		stage3_->position_ = { 840.0f - 32.0f,360.0f - 32.0f };
	}
	else if(stageNumber_ == 2) {
		stage1_->position_ = { 440.0f - 32.0f,360.0f - 32.0f };
		stage2_->position_ = { 640.0f - 32.0f,310.0f - 32.0f };
		stage3_->position_ = { 840.0f - 32.0f,360.0f - 32.0f };
	}
	else if (stageNumber_ == 3) {
		stage1_->position_ = { 440.0f - 32.0f,360.0f - 32.0f };
		stage2_->position_ = { 640.0f - 32.0f,360.0f - 32.0f };
		stage3_->position_ = { 840.0f - 32.0f,310.0f - 32.0f };
	}

	num_->uvTranslate_.x = float(stageNumber_) * 0.1f;

	skyDome_->SetColor(CreateColor(stageNumber_));

}

void SelectScene::DrawModel() {

	skyDome_->Draw(camera_.get());

}

void SelectScene::DrawSkinningModel() {

	

}

void SelectScene::DrawParticle() {



}

void SelectScene::DrawSprite() {

	stageSelect_->Draw();
	toTitle_->Draw();
	aButton_->Draw();
	/*stage1_->Draw();
	stage2_->Draw();
	stage3_->Draw();*/
	num_->Draw();

}

void SelectScene::DrawLine() {

}
