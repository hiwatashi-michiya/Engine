#include "GameScene.h"

#ifdef _DEBUG

#include "Engine/manager/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{
	plane_.reset(Model::Create("plane"));
	plane2_.reset(Model::Create("fence"));
}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	Model::worldTransformCamera_.translation_ = { 0.0f,0.0f,-10.0f };
	Model::worldTransformCamera_.rotation_.x = 0.0f;

	worldTransformPlane_.translation_.x = -2.0f;
	worldTransformPlane2_.translation_.x = 2.0f;

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &Model::worldTransformCamera_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &Model::worldTransformCamera_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &Model::worldTransformCamera_.translation_.x, 0.1f);
	ImGui::End();

	Model::StaticImGuiUpdate();

	plane_->ImGuiUpdate();
	plane2_->ImGuiUpdate();

#endif // _DEBUG

	worldTransformPlane_.UpdateMatrix();
	worldTransformPlane2_.UpdateMatrix();

}

void GameScene::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//3Dモデル描画
	{

		Model::PreDraw(commandList);

		plane_->Draw(worldTransformPlane_);
		plane2_->Draw(worldTransformPlane2_);

		Model::PostDraw();

	}
	
	//2Dスプライト描画
	{

		Sprite::PreDraw(commandList);



		Sprite::PostDraw();

	}


}
