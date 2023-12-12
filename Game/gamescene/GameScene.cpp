#include "GameScene.h"

#ifdef _DEBUG

#include "Engine/manager/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{


}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	Model::worldTransformCamera_.translation_ = { 0.0f,0.0f,-70.0f };
	Model::worldTransformCamera_.rotation_.x = 0.0f;

	player_ = std::make_unique<Player>();

	player_->Initialize();

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &Model::worldTransformCamera_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &Model::worldTransformCamera_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &Model::worldTransformCamera_.translation_.x, 0.1f);
	ImGui::End();

#endif // _DEBUG

	player_->Update();

}

void GameScene::Draw() {

	player_->Draw();

}
