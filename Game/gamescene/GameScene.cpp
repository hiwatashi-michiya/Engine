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
	Model::worldTransformCamera_.rotation_.x = 0.2f;

	player_ = std::make_unique<Player>();

	player_->Initialize();

	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	for (uint32_t i = 0; i < 11; i++) {
		std::shared_ptr<Block> block = std::make_shared<Block>();
		block->Initialize({ -25.0f + i * 5.0f, 3.0f, -25.0f + i * 5.0f }, player_.get(), { 2.0f,3.0f,4.0f });
		blocks_.push_back(block);
	}

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &Model::worldTransformCamera_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &Model::worldTransformCamera_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &Model::worldTransformCamera_.translation_.x, 0.1f);
	ImGui::End();

#endif // _DEBUG

	blocks_.remove_if([](auto& block) {

		if (block->GetIsDead()) {
			return true;
		}

		return false;

		});

	if (input_->GetIsGamepad()) {

		/*if (isLockOnStart_) {
			destinationAngleY_ = std::acosf(Model::worldTransformCamera_.rotateMatrix_.m[0][0]);
			isLockOnStart_ = false;

		}*/

		float rotateSpeed = 0.000002f;

		destinationAngleY_ += float(input_->GetGamepad().sThumbRX) * rotateSpeed;

		destinationAngleX_ += -float(input_->GetGamepad().sThumbRY) * rotateSpeed;

		if (destinationAngleX_ < -0.0f) {
			destinationAngleX_ = -0.0f;
		}
		else if (destinationAngleX_ > 0.2f) {
			destinationAngleX_ = 0.2f;
		}

		/*if (destinationAngleY_ > 3.14f) {
			destinationAngleY_ = -3.13f;
		}
		else if (destinationAngleY_ <= -3.14f) {
			destinationAngleY_ = 3.14f;
		}*/

		//右スティック押し込みでリセット
		if (input_->GetGamepad().wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			destinationAngleY_ = 0.0f;
			destinationAngleX_ = 0.2f;
		}

	}

	if (delay_ > 0.01f) {
		//最短角度補間
		Model::worldTransformCamera_.rotation_.y = std::lerp(Model::worldTransformCamera_.rotation_.y, destinationAngleY_, 1.0f / delay_);
		Model::worldTransformCamera_.rotation_.x = std::lerp(Model::worldTransformCamera_.rotation_.x, destinationAngleX_, 1.0f / delay_);
	}
	else {
		//最短角度補間
		Model::worldTransformCamera_.rotation_.y = std::lerp(Model::worldTransformCamera_.rotation_.y, destinationAngleY_, 1.0f);
		Model::worldTransformCamera_.rotation_.x = std::lerp(Model::worldTransformCamera_.rotation_.x, destinationAngleX_, 1.0f);
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
	stage_->Update();

	for (auto& block : blocks_) {
		block->Update();
	}

}

void GameScene::Draw() {

	player_->Draw();
	stage_->Draw();

	for (auto& block : blocks_) {
		block->Draw();
	}

}

Vector3 GameScene::CalcOffset() {

	Vector3 offset = { 0.0f, 10.0f, -70.0f };

	Matrix4x4 matRotate = MakeIdentity4x4();

	matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);

	/*Matrix4x4 tmprotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);*/

	//オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, matRotate);

	return offset;

}
