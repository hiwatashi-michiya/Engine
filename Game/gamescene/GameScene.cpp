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

	if (input_->GetIsGamepad()) {

		/*if (isLockOnStart_) {
			destinationAngleY_ = std::acosf(Model::worldTransformCamera_.rotateMatrix_.m[0][0]);
			isLockOnStart_ = false;

		}*/

		float rotateSpeed = 0.000001f;

		destinationAngleY_ += float(input_->GetGamepad().sThumbRX) * rotateSpeed;

		/*if (destinationAngleY_ > 3.14f) {
			destinationAngleY_ = -3.13f;
		}
		else if (destinationAngleY_ <= -3.14f) {
			destinationAngleY_ = 3.14f;
		}*/

		//右スティック押し込みでリセット
		if (input_->GetGamepad().wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			destinationAngleY_ = 0.0f;
		}

	}

	player_->Update();

}

void GameScene::Draw() {

	player_->Draw();

}
