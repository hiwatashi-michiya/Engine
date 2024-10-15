#include "TestScene.h"
#include "FrameWork/SceneManager.h"
#include "PostEffectDrawer.h"
#include <cmath>

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

TestScene::TestScene()
{

}

TestScene::~TestScene()
{
}

void TestScene::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();
	audioManager_ = AudioManager::GetInstance();

	editor_ = MapEditor::GetInstance();
	editor_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,2.0f, -20.0f };
	camera_->rotation_.y = 0.0f;

	loader_ = LevelDataLoader::GetInstance();
	loader_->Load("./Resources/Levels/scene.json");

	for (auto& object : loader_->levelData_->objects_) {

		if (object.type.compare("MESH") == 0) {
			std::shared_ptr<Model> newModel;

			newModel.reset(Model::Create(object.fileName));

			models_.push_back(newModel);

			std::shared_ptr<Transform> newTransform = std::make_shared<Transform>();
			newTransform->translate_ = object.translation;
			newTransform->rotateQuaternion_ = ConvertFromEuler(object.rotation);
			newTransform->scale_ = object.scaling;

			transforms_.push_back(newTransform);
		}
		if (object.type.compare("CAMERA") == 0) {
			camera_->position_ = object.translation;
			camera_->rotation_ = object.rotation;
		}

	}

	editor_->SetCamera(camera_.get());

	test_ = audioManager_->LoadInMF("./Resources/SE/test.mp3");

	line_ = std::make_unique<Line>();
	line_->start_ = { 0.0f,0.0f,0.0f };
	line_->end_ = { 0.0f,0.0f,0.0f };

}

void TestScene::Update() {

#ifdef _DEBUG

	Vector3 moves{};

	moves.x = input_->GetMouseMove().x;
	moves.y = input_->GetMouseMove().y;
	moves.z = float(input_->GetMouseWheel());

	ImGui::Begin("mouse");
	ImGui::Text("Pos x : %1.2f,  y : %1.2f", input_->GetMousePosition().x, input_->GetMousePosition().y);
	ImGui::Text("move & wheel move x : %1.2f, y : %1.2f, wheel : %1.2f", moves.x, moves.y, moves.z);

	if (input_->PushMouse(Input::kLeft)) {
		ImGui::Text("Left Push");
	}
	if (input_->PushMouse(Input::kRight)) {
		ImGui::Text("Right Push");
	}
	if (input_->PushMouse(Input::kCenter)) {
		ImGui::Text("Center Push");
	}

	if (input_->TriggerMouse(Input::kLeft) || input_->TriggerMouse(Input::kRight)
		|| input_->TriggerMouse(Input::kCenter)) {
		ImGui::Text("Trigger");
	}

	if (input_->ReleaseMouse(Input::kLeft) || input_->ReleaseMouse(Input::kRight)
		|| input_->ReleaseMouse(Input::kCenter)) {
		ImGui::Text("Release");
	}

	ImGui::End();

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("Frame Late");
	ImGui::Text("%1.2f", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Scene Change");
	ImGui::Text("Key 1 or 3 + L_ctrl: Change Scene\n1 : title\n3 : game");
	ImGui::End();

	if (input_->TriggerKey(DIK_1) and input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else if (input_->TriggerKey(DIK_3) and input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	else if (input_->TriggerKey(DIK_4) and input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("EDITOR");
	}

#endif // _DEBUG

	for (int32_t i = 0; i < models_.size(); i++) {
		transforms_[i]->UpdateMatrix();
		models_[i]->SetWorldMatrix(transforms_[i]->worldMatrix_);
	}

	/*editor_->Edit();*/

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();


}

void TestScene::DrawModel() {

	/*editor_->Draw(camera_.get());*/

	for (int32_t i = 0; i < models_.size(); i++) {
		models_[i]->Draw(camera_.get());
	}

}

void TestScene::DrawSkinningModel() {



}

void TestScene::DrawParticle() {



}

void TestScene::DrawSprite() {



}

void TestScene::DrawLine() {

}
