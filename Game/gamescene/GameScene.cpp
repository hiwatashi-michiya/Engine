#include "GameScene.h"

#ifdef _DEBUG

#include "Engine/manager/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

	tex_ = TextureManager::GetInstance()->Load("./resources/test.png");

	plane_.reset(Model::Create("./resources/plane/plane.obj"));
	plane2_.reset(Model::Create("./resources/plane/plane.obj"));
	plane3_.reset(Model::Create("./resources/plane/plane.obj"));
	plane4_.reset(Model::Create("./resources/sphere/sphere.obj"));
	plane5_ = plane4_.get();
	particle_.reset(Particle3D::Create("./resources/plane/plane.obj", 10));
	sprite_.reset(Sprite::Create(tex_, { 100.0f,100.0f }));

	for (uint32_t i = 0; i < 100; i++) {

		planes_[i].reset(Model::Create("./resources/plane/plane.obj"));

	}

	for (uint32_t i = 0; i < 10; i++) {
		
		WorldTransform worldTransform{};
		worldTransform.translation_ = { 0.1f * i + 1.0f, 0.1f * i + 0.0f, 1.0f };
		particleTransforms_.push_back(worldTransform);

	}

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
	worldTransformPlane2_.translation_.y = 1.0f;
	worldTransformPlane3_.translation_.x = 1.5f;
	worldTransformPlane4_.translation_.x = 1.0f;
	worldTransformPlane5_.translation_.x = -1.0f;

	for (uint32_t i = 0; i < 100; i++) {
		worldTransforms_[i].translation_.x = 0.5f * i;
	}

	audioManager_ = AudioManager::GetInstance();
	audio_ = audioManager_->SoundLoadWave("./resources/tempo_02.wav");
	audio2_ = audioManager_->SoundLoadWave("./resources/tempo_02.wav");
}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &Model::worldTransformCamera_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &Model::worldTransformCamera_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &Model::worldTransformCamera_.translation_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("worldTransform sphere");
	ImGui::DragFloat3("translation", &worldTransformPlane5_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &worldTransformPlane5_.rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &worldTransformPlane5_.scale_.x, 0.1f);
	ImGui::End();

	if (input_->TriggerKey(DIK_SPACE)) {
		audioManager_->Play(audio_, 0.2f);
	}
	Model::StaticImGuiUpdate();
	Particle3D::StaticImGuiUpdate();

	if (input_->TriggerKey(DIK_1)) {
		audioManager_->Play(audio_, 0.2f, true);
	}
	plane_->ImGuiUpdate();
	plane2_->ImGuiUpdate();
	sprite_->ImGuiUpdate("test");

#endif // _DEBUG

	worldTransformPlane_.UpdateMatrix();
	worldTransformPlane2_.UpdateMatrix();
	worldTransformPlane3_.UpdateMatrix();
	worldTransformPlane4_.UpdateMatrix();
	worldTransformPlane5_.UpdateMatrix();

	for (uint32_t i = 0; i < 100; i++) {
		worldTransforms_[i].UpdateMatrix();
	}

	for (WorldTransform& worldTransform : particleTransforms_) {
		/*worldTransform.translation_.y += 0.01f;*/
		worldTransform.UpdateMatrix();
	}

	if (input_->TriggerKey(DIK_Q)) {
		plane_->SetMesh("./resources/sphere/sphere.obj");
	}

	if (input_->TriggerKey(DIK_E)) {
		plane_->SetMesh("./resources/suzanne/suzanne.obj");
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
		audioManager_->ReStart(audio_);
	}

}

void GameScene::Draw() {

	plane_->Draw(worldTransformPlane_);
	plane2_->Draw(worldTransformPlane2_);
	plane3_->Draw(worldTransformPlane3_);
	plane4_->Draw(worldTransformPlane4_);
	plane5_->Draw(worldTransformPlane5_);
	
	particle_->Draw(particleTransforms_);

	for (uint32_t i = 0; i < 100; i++) {
		planes_[i]->Draw(worldTransforms_[i]);
	}

	sprite_->Draw();

}
