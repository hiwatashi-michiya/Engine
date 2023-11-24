#include "GameScene.h"
#include <cmath>
#include <algorithm>
#include "Engine/GlobalVariables/GlobalVariables.h"

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
	Model::worldTransformCamera_.translation_ = { 0.0f,15.0f,-50.0f };
	Model::worldTransformCamera_.rotation_.x = 0.25f;
	stage_ = std::make_unique<Stage>();
	stage_->Initialize();
	player_ = std::make_unique<Player>();
	player_->Initialize();

	for (uint32_t i = 0; i < 5; i++) {
		std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>();
		enemy->Initialize();
		enemy->SetPosition({ i * 10.0f - 20.0f, 0.0f,25.0f + i * 0.5f });
		enemy->SetVelocity({ 0.0f,0.0f,-0.1f + i * -0.1f });
		enemy->SetMaxMoveTimer(60);
		enemies_.push_back(enemy);

	}

	lockOn_ = std::make_unique<LockOn>();
	//初期化
	lockOn_->Initialize();
	player_->SetLockOn(lockOn_.get());

	modelSkydome_.reset(Model::Create("skydome"));
	worldTransformSkydome_.scale_ *= 500.0f;
	const char* groupName = "Camera";
	GlobalVariables::GetInstance()->AddItem(groupName, "Delay", delay_);


	ResetCamera();

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &Model::worldTransformCamera_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &Model::worldTransformCamera_.rotation_.x, 0.01f);
	ImGui::DragFloat3("translation", &Model::worldTransformCamera_.translation_.x, 0.1f);
	ImGui::End();

#endif // _DEBUG

	if (player_->GetIsFall()) {
		ResetEnemy();
	}

	//enemies_.remove_if([](std::shared_ptr<Enemy> enemy) {

	//	if (enemy->GetIsDead()) {
	//		return true;
	//	}

	//	return false;

	//});

	UpdateGlobalVariables();

	player_->Update();

	if (lockOn_->IsLockOn()) {

		//ロックオン座標
		Vector3 lockOnPos = lockOn_->GetTargetPosition();

		//追従対象からロックオン対象へのベクトル
		Vector3 sub = lockOnPos - player_->GetWorldPosition();

		destinationAngleY_ = std::atan2(sub.x, sub.z);

	}
	else if (input_->GetIsGamepad()) {

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
			interTarget_ = Lerp(interTarget_, player_->GetWorldPosition(), 1.0f / delay_);
		}
		else {
			interTarget_ = Lerp(interTarget_, player_->GetWorldPosition(), 1.0f);
		}
		
	}

	Vector3 tmpOffset = CalcOffset();

	Model::worldTransformCamera_.translation_ = interTarget_ + tmpOffset;

	for (auto& enemy : enemies_) {

		if (enemy->GetIsDead() == false) {
			enemy->Update();

			enemy->Collision(player_.get());
		}

	}

	//ロックオン更新
	lockOn_->Update(enemies_, Model::matView_);

	stage_->Update();

	stage_->Collision(player_.get(), enemies_);

	worldTransformSkydome_.UpdateMatrix();

}

void GameScene::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//3Dモデル描画
	{

		Model::PreDraw(commandList);

		modelSkydome_->Draw(worldTransformSkydome_);

		player_->Draw();

		for (auto& enemy : enemies_) {

			if (enemy->GetIsDead() == false) {
				enemy->Draw();
			}

		}

		stage_->Draw();

		Model::PostDraw();

	}
	
	//エフェクト描画
	{

		Particle3D::PreDraw(commandList);

		player_->DrawEffect();

		Particle3D::PostDraw();

	}

	//2Dスプライト描画
	{

		Sprite::PreDraw(commandList);

		lockOn_->Draw();

		Sprite::PostDraw();

	}


}

void GameScene::ResetCamera() {

	if (player_) {

		interTarget_ = player_->GetWorldPosition();
		Model::worldTransformCamera_.rotation_.y = player_->GetRotation().y;

	}

	player_->destinationAngleY_ = Model::worldTransformCamera_.rotation_.y;

	Vector3 tmpOffset = CalcOffset();

	Model::worldTransformCamera_.translation_ = interTarget_ + tmpOffset;

	destinationAngleY_ = 0.0f;

	//if (player_) {

	//	//回転速度
	//	float rotateSpeed = 0.001f;

	//	Model::worldTransformCamera_.rotation_.y += input_->GetGamepad().sThumbRX * rotateSpeed * rotateSpeed;

	//	Vector3 offset = { 0.0f, 10.0f, -30.0f };

	//	Matrix4x4 matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);

	//	//オフセットをカメラの回転に合わせて回転させる
	//	offset = TransformNormal(offset, matRotate);

	//	Model::worldTransformCamera_.translation_ = player_->GetWorldTransform().translation_ + offset;
	//}

}

void GameScene::ResetEnemy() {

	/*enemies_.clear();*/

	for (uint32_t i = 0; auto& enemy : enemies_) {

		//復活処理
		if (enemy->GetIsDead()) {
			enemy->Initialize();
			enemy->SetPosition({ i * 10.0f - 20.0f, 0.0f,25.0f + i * 0.5f });
			enemy->SetVelocity({ 0.0f,0.0f,-0.1f + i * -0.1f });
			enemy->ResetMoveTimer();
			enemy->SetIsDead(false);
		}

		i++;

	}

}

Vector3 GameScene::CalcOffset() {

	Vector3 offset = { 0.0f, 12.0f, -30.0f };

	Matrix4x4 matRotate = MakeRotateYMatrix(Model::worldTransformCamera_.rotation_.y);

	//オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, matRotate);

	return offset;

}

void GameScene::UpdateGlobalVariables() {

	const char* groupName = "Camera";
	delay_ = GlobalVariables::GetInstance()->GetFloatValue(groupName, "Delay");

}
