#include "GameScene.h"

#ifdef _DEBUG

#include "Engine/manager/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

	tex_ = TextureManager::GetInstance()->Load("./resources/plane/uvChecker.png");
	tex2_ = TextureManager::GetInstance()->Load("./resources/Texture/block.png");
	model_.reset(Model::Create("./resources/cube/cube.obj"));
	model2_.reset(Model::Create("./resources/cube/cube.obj"));

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

	model_->position_.x = 2.0f;

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,10.0f, -30.0f };
	camera_->rotation_.x = 0.3f;
	
}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();


#endif // _DEBUG

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

	mapEditor_->Edit();

	if (input_->TriggerKey(DIK_Q)) {
		model2_->SetTexture(tex_);
	}


}

void GameScene::Draw() {

	mapEditor_->Draw(camera_.get());

}
