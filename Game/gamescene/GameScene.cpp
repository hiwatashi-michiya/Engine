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

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,0.0f,-70.0f };
	camera_->rotation_.x = 0.2f;
	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

	player_ = std::make_unique<Player>();

	player_->Initialize();
	player_->SetCamera(camera_.get());

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();
	enemy_->SetPlayer(player_.get());
	enemy_->SetBlockList(&blocks_);

	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	tex_ = TextureManager::GetInstance()->Load("./resources/reticle.png");

	reticle_.reset(Sprite::Create(tex_, { 0.0f,0.0f }));

	reticle3D_.reset(Model::Create("./resources/cube/cube.obj"));
	reticle3D_->scale_ *= 0.0f;

	blocks_.clear();
	bullets_.clear();

	/*for (uint32_t i = 0; i < 11; i++) {
		std::shared_ptr<Block> block = std::make_shared<Block>();
		block->Initialize({ -25.0f + i * 5.0f, 3.0f, -25.0f + i * 5.0f }, player_.get(), { 2.0f,3.0f,4.0f });
		blocks_.push_back(block);
	}*/

}

void GameScene::Reset() {

	camera_->position_ = { 0.0f,0.0f,-70.0f };
	camera_->rotation_.x = 0.2f;
	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

	player_->Initialize();
	player_->SetCamera(camera_.get());

	enemy_->Initialize();
	enemy_->SetPlayer(player_.get());
	enemy_->SetBlockList(&blocks_);

	stage_->Initialize();

	blocks_.clear();
	bullets_.clear();

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

#endif // _DEBUG

	if (input_->TriggerKey(DIK_R)) {
		Reset();
	}

	blocks_.remove_if([](auto& block) {

		if (block->GetIsDead()) {
			return true;
		}

		return false;

	});

	bullets_.remove_if([](auto& bullet) {

		if (bullet->GetIsDead()) {
			return true;
		}

		return false;

	});

	if (input_->GetIsGamepad()) {

		float rotateSpeed = 0.000002f;

		destinationAngleY_ += float(input_->GetGamepad().sThumbRX) * rotateSpeed;

		destinationAngleX_ += -float(input_->GetGamepad().sThumbRY) * rotateSpeed;

		if (destinationAngleX_ < -0.0f) {
			destinationAngleX_ = -0.0f;
		}
		else if (destinationAngleX_ > 0.2f) {
			destinationAngleX_ = 0.2f;
		}

		//右スティック押し込みでリセット
		if (input_->GetGamepad().wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			destinationAngleY_ = 0.0f;
			destinationAngleX_ = 0.2f;
		}

	}

	if (delay_ > 0.01f) {
		//最短角度補間
		camera_->rotation_.y = std::lerp(camera_->rotation_.y, destinationAngleY_, 1.0f / delay_);
		camera_->rotation_.x = std::lerp(camera_->rotation_.x, destinationAngleX_, 1.0f / delay_);
	}
	else {
		//最短角度補間
		camera_->rotation_.y = std::lerp(camera_->rotation_.y, destinationAngleY_, 1.0f);
		camera_->rotation_.x = std::lerp(camera_->rotation_.x, destinationAngleX_, 1.0f);
	}

	//レティクル
	{

		//カメラからの距離
		const float kDistance = 120.0f;
		//オフセット
		Vector3 offset = { 0.0f,0.0f,1.0f };
		offset = TransformNormal(offset, camera_->matWorld_);
		offset = Normalize(offset) * kDistance;
		reticlePos_ = camera_->position_ + offset;
		reticle3D_->position_ = reticlePos_;

	}

	{

		Vector3 posReticle = { reticle3D_->matWorld_.m[3][0],reticle3D_->matWorld_.m[3][1],reticle3D_->matWorld_.m[3][2] };
		//ビューポート
		Matrix4x4 matViewport = MakeViewportMatrix(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);
		Matrix4x4 cameraMatrix = camera_->matWorld_;
		Matrix4x4 matView = Inverse(cameraMatrix);
		Matrix4x4 matProjection = MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 1000.0f);
		Matrix4x4 matViewProjectionViewport = matView * matProjection * matViewport;
		posReticle = CoordTransform(posReticle, matViewProjectionViewport);
		reticle_->position_ = { posReticle.x - 64.0f, posReticle.y - 64.0f };

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

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->position_ = interTarget_ + tmpOffset;

	camera_->Update();

	player_->Update();
	stage_->Update();

	enemy_->Update();

	for (auto& block : blocks_) {
		block->Update();

		if (IsCollision(block->GetCollision(), player_->GetCollision())) {

			//前フレームのZ位置がブロックのZ判定の内側だった場合、左右どちらかから当たっているので左右の処理をする
			if (player_->GetPrePosition().z < block->GetCollision().max.z &&
				player_->GetPrePosition().z > block->GetCollision().min.z) {

				if (player_->GetPosition().x > block->GetPosition().x) {
					player_->SetPosition(Vector3{ block->GetCollision().max.x + 1.0f, player_->GetLocalPosition().y,player_->GetLocalPosition().z });
				}
				else {
					player_->SetPosition(Vector3{ block->GetCollision().min.x - 1.0f, player_->GetLocalPosition().y,player_->GetLocalPosition().z });
				}

			}
			//そうでない場合、上下どちらかから当たっているのでその処理をする
			if (player_->GetPrePosition().x < block->GetCollision().max.x &&
				player_->GetPrePosition().x > block->GetCollision().min.x) {

				if (player_->GetPosition().z > block->GetPosition().z) {
					player_->SetPosition(Vector3{ player_->GetLocalPosition().x,player_->GetLocalPosition().y,block->GetCollision().max.z + 1.0f });
				}
				else {
					player_->SetPosition(Vector3{ player_->GetLocalPosition().x,player_->GetLocalPosition().y,block->GetCollision().min.z - 1.0f });
				}

			}

		}

		if (block->GetIsDead()) {
			block->SetBullet(bullets_);
		}

	}

	for (auto& bullet : bullets_) {

		bullet->Update();

		if (player_->GetIsAttack() && !bullet->GetIsShot()) {
			bullet->Shot(reticlePos_);
			/*player_->SetIsAttack(false);*/
		}

		if (!enemy_->GetIsDead() && bullet->GetIsShot() && IsCollision(enemy_->GetCollision(), bullet->GetCollision())) {
			enemy_->Damage(1);
			bullet->SetIsDead(true);
		}

	}

}

void GameScene::Draw() {

	player_->Draw();
	stage_->Draw(camera_.get());

	enemy_->Draw(camera_.get());

	reticle_->Draw();
	reticle3D_->Draw(camera_.get());

	for (auto& block : blocks_) {
		block->Draw(camera_.get());
	}

	for (auto& bullet : bullets_) {
		bullet->Draw(camera_.get());
	}

	player_->DrawTexture();
	enemy_->DrawTexture();

}

Vector3 GameScene::CalcOffset() {

	Vector3 offset = { 0.0f, 10.0f, -70.0f };

	Matrix4x4 matRotate = MakeIdentity4x4();

	matRotate = MakeRotateYMatrix(camera_->rotation_.y);

	/*Matrix4x4 tmprotate = MakeRotateYMatrix(camera_->rotation_.y);*/

	//オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, matRotate);

	return offset;

}
