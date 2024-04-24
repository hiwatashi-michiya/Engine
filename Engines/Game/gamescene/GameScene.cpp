#include "GameScene.h"
#include "FrameWork/SceneManager.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

	

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
	camera_->position_ = { 0.0f,44.0f, -33.0f };
	camera_->rotation_.x = 0.9f;
	mapEditor_->SetCamera(camera_.get());

	player_ = std::make_shared<Player>();
	player_->Initialize();

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	mapEditor_->Edit();

#endif // _DEBUG

	player_->Update();

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

}

void GameScene::DrawModel() {

	mapEditor_->Draw(camera_.get());

	player_->Draw(camera_.get());

}

void GameScene::DrawParticle() {



}

void GameScene::DrawSprite() {



}

