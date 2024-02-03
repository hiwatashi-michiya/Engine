#include "GameScene.h"

#ifdef _DEBUG



#endif // _DEBUG

#include "Engine/manager/ImGuiManager.h"

GameScene::GameScene()
{

	tex_ = TextureManager::GetInstance()->Load("./resources/plane/uvChecker.png");
	tex2_ = TextureManager::GetInstance()->Load("./resources/Texture/block.png");
	model_.reset(Model::Create("./resources/sphere/sphere.obj"));
	model2_.reset(Model::Create("./resources/plane/plane.obj"));

}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	model_->position_.x = 2.0f;

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,10.0f, -30.0f };
	camera_->rotation_.x = 0.3f;

}

void GameScene::Update() {

#ifdef _DEBUG

	

#endif // _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	model_->ImGuiUpdate("Ball");
	model2_->ImGuiUpdate("Plane");

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

	/*particle_.Update();*/

	if (input_->TriggerKey(DIK_Q)) {
		model2_->SetTexture(tex_);
	}

}

void GameScene::Draw() {

	model_->Draw(camera_.get());
	model2_->Draw(camera_.get());

}
