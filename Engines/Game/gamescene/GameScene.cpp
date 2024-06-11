#include "GameScene.h"
#include "SceneManager.h"
#include "PostEffectDrawer.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

	skyDome_.reset(Model::Create("./Resources/skydome/temp.obj"));
	pauseTex_ = TextureManager::GetInstance()->Load("./Resources/UI/pause.png");
	pauseSprite_.reset(Sprite::Create(pauseTex_, { 640.0f - 256.0f,360.0f - 64.0f }));

}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();
	gv_ = GlobalVariables::GetInstance();
	mapEditor_ = MapEditor::GetInstance();
	audioManager_ = AudioManager::GetInstance();

	mapEditor_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,35.0f, -30.0f };
	camera_->rotation_.x = 0.9f;
	mapEditor_->SetCamera(camera_.get());

	skyDomeTransform_ = std::make_unique<Transform>();
	skyDomeTransform_->scale_ = { 500.0f,500.0f,500.0f };
	skyDome_->material_->dLightMap_->direction = { 0.0f,0.0f,1.0f };
	skyDome_->material_->dLightMap_->intensity = 5.0f;
	skyDome_->material_->pLightMap_->radius = 650.0f;
	skyDome_->material_->pLightMap_->intensity = 0.0f;
	skyDome_->material_->pLightMap_->decay = 0.5f;

	stage_ = std::make_unique<Stage>();
	stage_->Initialize();
	stage_->LoadStage(1);

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	skyDome_->ImGuiUpdate("skyDome");

	ImGui::Begin("manual");
	ImGui::Text("Move : Left Stick");
	ImGui::Text("Reset : R key");
	ImGui::End();

	PostEffectDrawer::GetInstance()->Debug();

	if (input_->TriggerKey(DIK_R)) {
		stage_->Initialize();
		stage_->LoadStage(1);
		resetCount_ = 60;
	}

	/*mapEditor_->Edit();*/

#endif // _DEBUG

	

	if (isPause_) {

		if (input_->TriggerButton(Input::Button::START)) {
			isPause_ = false;
		}

		PostEffectDrawer::GetInstance()->SetType(kGaussianFilter);

	}
	else {

		if (input_->TriggerButton(Input::Button::START)) {
			isPause_ = true;
		}

		if (stage_->GetPlayer()->GetIsDead()) {

			resetCount_--;

			if (resetCount_ <= 0) {
				stage_->Initialize();
				stage_->LoadStage(1);
				resetCount_ = 60;
			}

			PostEffectDrawer::GetInstance()->SetType(kGrayscale);

		}
		else {
			PostEffectDrawer::GetInstance()->SetType(kNone);
		}

		stage_->Update();

		camera_->Update();

		skyDomeTransform_->UpdateMatrix();
		skyDome_->SetWorldMatrix(skyDomeTransform_->worldMatrix_);

	}

}

void GameScene::DrawModel() {

	skyDome_->Draw(camera_.get());

	/*mapEditor_->Draw(camera_.get());*/

	stage_->Draw(camera_.get());

}

void GameScene::DrawSkinningModel() {

	stage_->DrawSkinningModel(camera_.get());

}

void GameScene::DrawParticle() {



}

void GameScene::DrawSprite() {

	if (isPause_) {
		pauseSprite_->Draw();
	}

}

void GameScene::DrawLine() {

	stage_->DrawLine(camera_.get());

}
