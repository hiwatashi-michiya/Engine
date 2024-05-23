#include "SelectScene.h"
#include "FrameWork/SceneManager.h"
#include <cmath>

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

SelectScene::SelectScene()
{

}

SelectScene::~SelectScene()
{
}

void SelectScene::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();
	audioManager_ = AudioManager::GetInstance();

	editor_ = MapEditor::GetInstance();
	editor_->Initialize();

	loader_ = LevelDataLoader::GetInstance();
	loader_->Load("./Resources/Levels/parent.json");

	for (auto& object : loader_->levelData_->objects_) {

		std::shared_ptr<Model> newModel;

		newModel.reset(Model::Create("./Resources/cube/cube.obj"));
		
		models_.push_back(newModel);

		std::shared_ptr<Transform> newTransform = std::make_shared<Transform>();
		newTransform->translate_ = object.translation;
		newTransform->rotateQuaternion_ = ConvertFromEuler(object.rotation);
		newTransform->scale_ = object.scaling;

		transforms_.push_back(newTransform);

	}

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,2.0f, -20.0f };
	camera_->rotation_.y = 0.0f;

	editor_->SetCamera(camera_.get());

	model_.reset(SkinningModel::Create("./Resources/human/walking.gltf", 0));
	
	model_->LoadAnimation("./Resources/human/stay.gltf", 1);

	transform_ = std::make_unique<Transform>();

	test_ = audioManager_->LoadInMF("./Resources/SE/test.mp3");

	line_ = std::make_unique<Line>();
	line_->start_ = { 0.0f,0.0f,0.0f };
	line_->end_ = { 0.0f,0.0f,0.0f };

}

void SelectScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("Animation");
	ImGui::DragFloat("animation - Speed", &speed_, 0.01f);
	ImGui::End();

	ImGui::Begin("transform");
	ImGui::DragFloat3("scale", &transform_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &transform_->rotateQuaternion_.x, 0.1f);
	ImGui::DragFloat3("translation", &transform_->translate_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("Line");
	ImGui::DragFloat3("start", &line_->start_.x, 0.1f);
	ImGui::DragFloat3("end", &line_->end_.x, 0.1f);
	ImGui::End();

#endif // _DEBUG

	if (input_->TriggerKey(DIK_1)) {
		model_->StartAnimation();
	}

	if (input_->TriggerKey(DIK_2)) {
		model_->StopAnimation();
	}

	if (input_->TriggerKey(DIK_3)) {
		model_->ResetAnimation();
	}

	if (input_->TriggerKey(DIK_4)) {
		model_->SetAnimation(1);
	}

	if (input_->TriggerKey(DIK_5)) {
		model_->SetAnimation(0);
	}

	for (int32_t i = 0; i < transforms_.size(); i++) {
		transforms_[i]->UpdateMatrix();
	}

	for (int32_t i = 0; i < models_.size(); i++) {
		models_[i]->SetWorldMatrix(transforms_[i]->worldMatrix_);
	}

	model_->SetAnimationSpeed(speed_);

	transform_->UpdateMatrix();

	model_->SetWorldMatrix(transform_->worldMatrix_);

	model_->UpdateAnimation();

	editor_->Edit();

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

	if (input_->TriggerKey(DIK_1)) {
		audioManager_->Play(test_, 0.5f);
	}

}

void SelectScene::DrawModel() {

	editor_->Draw(camera_.get());

	for (int32_t i = 0; i < models_.size(); i++) {
		models_[i]->Draw(camera_.get());
	}

}

void SelectScene::DrawSkinningModel() {

	model_->Draw(camera_.get());

}

void SelectScene::DrawParticle() {



}

void SelectScene::DrawSprite() {



}

void SelectScene::DrawLine() {

	model_->DrawSkeleton(camera_.get());

}
