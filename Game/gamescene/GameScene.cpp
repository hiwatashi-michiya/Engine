#include "GameScene.h"

#ifdef _DEBUG

#include "Engine/manager/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

	tex_ = TextureManager::GetInstance()->Load("./resources/test.png");

}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	gv_ = GlobalVariables::GetInstance();
	mapEditor_ = MapEditor::GetInstance();

	mapEditor_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,10.0f, -30.0f };
	camera_->rotation_.x = 0.3f;
	
	gv_->AddItem(gName_, "test", obj_);

}

void GameScene::Update() {

	obj_ = gv_->GetObjectDataValue(gName_, "test");

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	mapEditor_->Edit();

#endif // _DEBUG

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

}

void GameScene::Draw() {

	mapEditor_->Draw(camera_.get());

}
