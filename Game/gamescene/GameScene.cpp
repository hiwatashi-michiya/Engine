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
	audioManager_ = AudioManager::GetInstance();
	Model::worldTransformCamera_.translation_ = { 0.0f,30.0f,-50.0f };
	Model::worldTransformCamera_.rotation_.x = 0.5f;
	audio_ = audioManager_->SoundLoadWave("./resources/tempo_02.wav");
}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &Model::worldTransformCamera_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &Model::worldTransformCamera_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &Model::worldTransformCamera_.translation_.x, 0.1f);
	ImGui::End();

#endif // _DEBUG

	if (input_->TriggerKey(DIK_SPACE)) {
		audioManager_->Play(audio_, 0.2f);
	}

	if (input_->TriggerKey(DIK_1)) {
		audioManager_->Play(audio_, 0.2f, true);
	}

	if (input_->TriggerKey(DIK_2)) {
		audioManager_->Stop(audio_);
	}

	if (input_->TriggerKey(DIK_3)) {
		audioManager_->Pause(audio_);
	}

	if (input_->TriggerKey(DIK_4)) {
		audioManager_->SetVolume(audio_, 0.4f);
	}

	if (input_->TriggerKey(DIK_5)) {
		audioManager_->SetVolume(audio_, 1.0f);
	}

}

void GameScene::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//3Dモデル描画
	{

		Model::PreDraw(commandList);

		

		Model::PostDraw();

	}
	
	//2Dスプライト描画
	{

		Sprite::PreDraw(commandList);



		Sprite::PostDraw();

	}


}
