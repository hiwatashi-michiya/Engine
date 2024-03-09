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

	model_.reset(Model::Create("./Resources/cube/cube.obj"));
	transform_ = std::make_unique<Transform>();
	model2_.reset(Model::Create("./Resources/sphere/sphere.obj"));
	model2_->material_->constMap_->isGrayScale = 1;
	transform2_ = std::make_unique<Transform>();
	transform2_->translate_ = { 3.0f,0.0f,0.0f };
	model3_.reset(Model::Create("./Resources/plane/plane.obj"));
	model3_->material_->constMap_->isRetro = 1;
	transform3_ = std::make_unique<Transform>();
	transform3_->translate_ = { -3.0f,0.0f,0.0f };
	transform3_->rotate_ = { 0.0f,3.14f,0.0f };
	model4_.reset(Model::Create("./Resources/cube/cube.obj"));
	model4_->material_->pLightMap_->color = { 1.0f,0.5f,0.0f,1.0f };
	transform4_ = std::make_unique<Transform>();
	transform4_->translate_ = { 8.5f,3.0f,-5.0f };
	model5_.reset(Model::Create("./Resources/cube/cube.obj"));
	model5_->material_->constMap_->isEmboss = 1;
	transform5_ = std::make_unique<Transform>();
	transform5_->translate_ = { -8.5f,3.0f,-5.0f };

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

	transform_->UpdateMatrix();
	transform2_->UpdateMatrix();
	transform3_->UpdateMatrix();
	transform4_->UpdateMatrix();
	transform5_->UpdateMatrix();

	model_->SetWorldMatrix(transform_->worldMatrix_);
	model2_->SetWorldMatrix(transform_->worldMatrix_);
	model3_->SetWorldMatrix(transform_->worldMatrix_);
	model4_->SetWorldMatrix(transform_->worldMatrix_);
	model5_->SetWorldMatrix(transform_->worldMatrix_);

}

void GameScene::Draw() {

	model_->Draw(camera_.get());
	model2_->Draw(camera_.get());
	model3_->Draw(camera_.get());
	model4_->Draw(camera_.get());
	model5_->Draw(camera_.get());

}
