#include "GameScene.h"
#include "FrameWork/SceneManager.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();
	editor_ = MapEditor::GetInstance();
	editor_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,10.0f, -30.0f };
	camera_->rotation_.x = 0.3f;

	model_.reset(Model::Create("./resources/cube/cube.obj"));
	model2_.reset(Model::Create("./resources/sphere/sphere.obj"));
	model2_->position_ = { 3.0f,0.0f,0.0f };
	model2_->material_->constMap_->isGrayScale = 1;
	model3_.reset(Model::Create("./resources/plane/plane.obj"));
	model3_->position_ = { -3.0f,0.0f,0.0f };
	model3_->rotation_ = { 0.0f,3.14f,0.0f };
	model3_->material_->constMap_->isRetro = 1;
	model4_.reset(Model::Create("./resources/cube/cube.obj"));
	model4_->position_ = { 8.5f,3.0f,-5.0f };
	model4_->material_->pLightMap_->color = { 1.0f,0.5f,0.0f,1.0f };
	model5_.reset(Model::Create("./resources/cube/cube.obj"));
	model5_->position_ = { -8.5f,3.0f,-5.0f };
	model5_->material_->constMap_->isEmboss = 1;

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("Manual");
	ImGui::Text("Key S + L_ctrl: Change Scene");
	ImGui::End();

#endif // _DEBUG

	if (input_->TriggerKey(DIK_S) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

}

void GameScene::Draw() {

	model_->Draw(camera_.get());
	model2_->Draw(camera_.get());
	model3_->Draw(camera_.get());
	model4_->Draw(camera_.get());
	model5_->Draw(camera_.get());

}
