#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	Model::worldTransformCamera_.translation_ = { 0.0f,0.0f,-10.0f };

	uvTex_ = TextureManager::GetInstance()->Load("./resources/uvChecker.png");
	uvSprite_.reset(Sprite::Create(uvTex_, { 0.0f,500.0f }));

	suzanneModel_.reset(Model::Create("suzanne"));

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("player");
	ImGui::DragFloat3("pos", &worldTransformSuzanne_.translation_.x, 0.1f);
	ImGui::End();

#endif // _DEBUG


	if (input_->PushKey(DIK_W)) {
		worldTransformSuzanne_.translation_.z += 0.1f;
	}

	if (input_->PushKey(DIK_S)) {
		worldTransformSuzanne_.translation_.z -= 0.1f;
	}

	worldTransformSuzanne_.UpdateMatrix();

}

void GameScene::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//3Dモデル描画
	{

		Model::PreDraw(commandList);

		suzanneModel_->Draw(worldTransformSuzanne_);

		Model::PostDraw();

		Sprite::PreDraw(commandList);

		uvSprite_->Draw();

		Sprite::PostDraw();

	}
	



}