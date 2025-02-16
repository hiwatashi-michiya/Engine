#include "SelectScene.h"
#include "FrameWork/SceneManager.h"
#include "PostEffectDrawer.h"
#include <cmath>
#include "Game/ColorSetter/ColorSetter.h"
#include "InstancingModel.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

SelectScene::SelectScene()
{
	stageSelectTex_ = TextureManager::GetInstance()->Load("./Resources/UI/stageSelect.png");
	toTitleTex_ = TextureManager::GetInstance()->Load("./Resources/UI/toTitle.png");
	numTex_ = TextureManager::GetInstance()->Load("./Resources/UI/num.png");
	buttonTex_ = TextureManager::GetInstance()->Load("./Resources/UI/a.png");
	skyDome_ = std::make_unique<Model>();
	skyDome_->Initialize("./Resources/skydome/temp.obj");
	stageSelect_.reset(Sprite::Create(stageSelectTex_, { 640.0f - 256.0f,100.0f - 32.0f }));
	toTitle_.reset(Sprite::Create(toTitleTex_, { 140.0f - 128.0f,60.0f - 32.0f }));
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

	particleManager_ = ParticleManager::GetInstance();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 15.0f,165.0f, -120.0f };
	camera_->rotation_.x = 0.9f;

	skyDomeTransform_ = std::make_unique<Transform>();
	skyDomeTransform_->scale_ = { 500.0f,500.0f,500.0f };
	skyDome_->GetInstancingModel()->material_->dLightMap_->direction = { 0.0f,-0.8f,0.6f };
	skyDome_->GetInstancingModel()->material_->dLightMap_->intensity = 1.0f;
	skyDome_->GetInstancingModel()->material_->pLightMap_->radius = 650.0f;
	skyDome_->GetInstancingModel()->material_->pLightMap_->intensity = 0.15f;
	skyDome_->GetInstancingModel()->material_->pLightMap_->decay = 0.5f;
	skyDomeTransform_->UpdateMatrix();
	skyDome_->SetWorldMatrix(skyDomeTransform_->worldMatrix_);

	LoadAllStage();

}

void SelectScene::Finalize()
{
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

	if (input_->TriggerKey(DIK_LEFT) or input_->TriggerKey(DIK_A)) {

		if (stageNumber_ > 1) {
			stageNumber_--;
		}

	}

	if (input_->TriggerKey(DIK_RIGHT) or input_->TriggerKey(DIK_D)) {

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

	//左の入力があったら数字を下げる
	if (input_->TriggerButton(Input::Button::LEFT) or input_->TriggerLStick(Input::StickArrow::S_LEFT)) {
		//ステージが1の場合は下げない
		if (stageNumber_ > 1) {
			stageNumber_--;
		}

	}
	//右の入力があったら数字を上げる
	if (input_->TriggerButton(Input::Button::RIGHT) or input_->TriggerLStick(Input::StickArrow::S_RIGHT)) {
		//最終ステージなら上げない
		if (stageNumber_ < kMaxStage_) {
			stageNumber_++;
		}

	}
	//Aボタンでステージに入る
	if (input_->TriggerButton(Input::Button::A) and !SceneChangeManager::GetInstance()->IsSceneChange()) {
		SceneChangeManager::GetInstance()->SetNextScene("GAMEPLAY");
		SceneChangeManager::GetInstance()->SceneChangeStart();
	}
	//Bボタンでタイトルへ
	if (input_->TriggerButton(Input::Button::B) and !SceneChangeManager::GetInstance()->IsSceneChange()) {
		SceneChangeManager::GetInstance()->SetNextScene("TITLE");
		SceneChangeManager::GetInstance()->SceneChangeStart();
	}

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

	//10分割した時のuvの値
	float division = 0.1f;

	num_->uvTranslate_.x = float(stageNumber_) * division;

	skyDome_->SetColor(CreateColor(GameColor::Color(stageNumber_)));

	for (int32_t i = 0; i < gameObjects_[stageNumber_ - 1].size(); i++) {
		gameObjects_[stageNumber_ - 1][i]->Update();
	}

	particleManager_->Update();

}

void SelectScene::Draw()
{

	for (int32_t i = 0; i < gameObjects_[stageNumber_ - 1].size(); i++) {
		gameObjects_[stageNumber_ - 1][i]->Draw(camera_.get());
	}

	particleManager_->Draw(camera_.get());

	skyDome_->Draw(camera_.get());

	stageSelect_->Draw();
	toTitle_->Draw();
	aButton_->Draw();
	num_->Draw();

}

void SelectScene::LoadAllStage()
{

	StageLoader loader;

	for (int32_t i = 0; i < kMaxStage_; i++) {
		loader.LoadStage(i + 1, gameObjects_[i]);
	}

}
