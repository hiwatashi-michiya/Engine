#include "GameScene.h"

#ifdef _DEBUG

#include "Engine/manager/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

	tex_ = TextureManager::GetInstance()->Load("./resources/test.png");

	plane_.reset(Model::Create("./resources/sphere/sphere.obj"));
	plane2_.reset(Model::Create("./resources/terrain/terrain.obj"));
	plane3_.reset(Model::Create("./resources/plane/plane.obj"));
	plane4_.reset(Model::Create("./resources/sphere/sphere.obj"));
	plane5_ = plane4_.get();
	particle_.reset(Particle3D::Create("./resources/plane/plane.obj", 10));
	sprite_.reset(Sprite::Create(tex_, { 100.0f,100.0f }));

	for (uint32_t i = 0; i < 100; i++) {

		planes_[i].reset(Model::Create("./resources/plane/plane.obj"));

	}

	for (uint32_t i = 0; i < 10; i++) {
		
		particle_->positions_[i] = {0.1f * i + 1.0f, 0.1f * i + 0.0f, 1.0f};

	}

}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,0.0f, -10.0f };

	plane_->position_.x = -2.0f;
	plane2_->position_.y = -2.0f;
	plane3_->position_.x = 1.5f;
	plane4_->position_.x = 1.0f;
	plane5_->position_.x = -1.0f;

	for (uint32_t i = 0; i < 100; i++) {
		planes_[i]->position_.x = 0.5f * i;
		planes_[i]->rotation_.z = 0.05f * i;
	}

	audioManager_ = AudioManager::GetInstance();
	audio_ = audioManager_->SoundLoadWave("./resources/tempo_02.wav");
	audio2_ = audioManager_->SoundLoadWave("./resources/tempo_02.wav");
}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("worldTransform sphere");
	ImGui::DragFloat3("translation", &plane5_->position_.x, 0.1f);
	ImGui::DragFloat3("rotation", &plane5_->rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &plane5_->scale_.x, 0.1f);
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

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

}

void GameScene::Draw() {

	plane_->Draw(camera_.get());
	plane2_->Draw(camera_.get());
	/*plane3_->Draw();
	plane4_->Draw();
	plane5_->Draw();*/
	
	particle_->Draw(camera_.get());

	/*for (uint32_t i = 0; i < 100; i++) {
		planes_[i]->Draw();
	}*/

	sprite_->Draw();

}
