#include "DebugCamera.h"
#include "ImGuiManager.h"

DebugCamera::DebugCamera()
{
	camera_ = std::make_shared<Camera>();
}

DebugCamera::~DebugCamera()
{
}

void DebugCamera::Initialize() {

	input_ = Input::GetInstance();

	camera_->Initialize();
	camera_->position_ = { 0.0f,65.0f, -60.0f };
	camera_->rotation_.x = 0.9f;

}

void DebugCamera::Update() {

	if (input_->PushMouse(Input::Mouse::kLeft)) {

		if (input_->GetMouseMove().x != 0.0f) {

			camera_->rotation_.y += input_->GetMouseMove().x * 0.001f;

		}

		if (input_->GetMouseMove().y != 0.0f) {

			camera_->rotation_.x += input_->GetMouseMove().y * 0.001f;

		}

	}

	else if (input_->PushMouse(Input::Mouse::kRight)) {

		if (input_->GetMouseMove().x != 0.0f) {

			camera_->position_.x -= input_->GetMouseMove().x * 0.01f;

		}

		if (input_->GetMouseMove().y != 0.0f) {

			camera_->position_.y -= input_->GetMouseMove().y * 0.01f;

		}

	}

	if (input_->GetMouseWheel() != 0.0f) {

		camera_->position_.z += float(input_->GetMouseWheel()) * 0.01f;

	}

	Quaternion cameraQuaternion = IdentityQuaternion();

	cameraQuaternion = cameraQuaternion.ConvertFromEuler(camera_->rotation_);

	camera_->matRotate_ = MakeRotateMatrix(cameraQuaternion);

	camera_->Update();

}

void DebugCamera::Debug() {

#ifdef _DEBUG

	ImGui::Begin("Debug Camera");

	ImGui::End();

#endif // _DEBUG


}
