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
	Model::worldTransformCamera_.translation_ = { 0.0f,10.0f,-50.0f };
	Model::worldTransformCamera_.rotation_.x = 0.25f;
	stage_ = std::make_unique<Stage>();
	stage_->Initialize();
	player_ = std::make_unique<Player>();
	player_->Initialize();
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();
	modelSkydome_.reset(Model::Create("skydome"));
	worldTransformSkydome_.scale_ *= 500.0f;

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

	if (player_) {

		//回転速度
		float rotateSpeed = 0.001f;

		Model::worldTransformCamera_.rotation_.y += input_->GetGamepad().sThumbRX * rotateSpeed * rotateSpeed;

		Vector3 offset = { 0.0f, 10.0f, -30.0f };

		Matrix4x4 matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);

		//オフセットをカメラの回転に合わせて回転させる
		offset = TransformNormal(offset, matRotate);

		Model::worldTransformCamera_.translation_ = player_->GetWorldTransform().translation_ + offset;
	}

	enemy_->Update();

	enemy_->Collision(player_.get());

	stage_->Update();

	stage_->Collision(player_.get(), enemy_.get());

	worldTransformSkydome_.UpdateMatrix();

}

void GameScene::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//3Dモデル描画
	{

		Model::PreDraw(commandList);

		modelSkydome_->Draw(worldTransformSkydome_);

		player_->Draw();

		enemy_->Draw();

		stage_->Draw();

		Model::PostDraw();

	}
	
	//2Dスプライト描画
	{

		Sprite::PreDraw(commandList);



		Sprite::PostDraw();

	}


}
