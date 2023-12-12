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

	if (delay_ > 0.01f) {
		//最短角度補間
		Model::worldTransformCamera_.rotation_.y = std::lerp(Model::worldTransformCamera_.rotation_.y, destinationAngleY_, 1.0f / delay_);
	}
	else {
		//最短角度補間
		Model::worldTransformCamera_.rotation_.y = std::lerp(Model::worldTransformCamera_.rotation_.y, destinationAngleY_, 1.0f);
	}

	if (player_) {

		if (delay_ > 0.01f) {
			interTarget_ = Lerp(interTarget_, player_->GetPosition(), 1.0f / delay_);
		}
		else {
			interTarget_ = Lerp(interTarget_, player_->GetPosition(), 1.0f);
		}

	}

	Vector3 tmpOffset = CalcOffset();

	Model::worldTransformCamera_.translation_ = interTarget_ + tmpOffset;

	Model::worldTransformCamera_.UpdateMatrix();

	player_->Update();

}

void GameScene::Draw() {

	player_->Draw();

}

Vector3 GameScene::CalcOffset() {

	Vector3 offset = { 0.0f, 5.0f, -30.0f };

	Matrix4x4 matRotate = MakeIdentity4x4();

	matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);

	/*Matrix4x4 tmprotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);*/

	//オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, matRotate);

	return offset;

}
