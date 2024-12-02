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

	if (input_->PushMouse(Input::Mouse::kCenter)) {

		if (input_->GetMouseMove().x != 0.0f) {

			camera_->rotation_.y += input_->GetMouseMove().x * rotateSensi_;

		}

		if (input_->GetMouseMove().y != 0.0f) {

			camera_->rotation_.x += input_->GetMouseMove().y * rotateSensi_;

		}

	}

	else if (input_->PushMouse(Input::Mouse::kRight)) {

		if (input_->GetMouseMove().x != 0.0f) {

			camera_->position_ -= camera_->matWorld_.GetXAxis() * input_->GetMouseMove().x * moveSensi_;

		}

		if (input_->GetMouseMove().y != 0.0f) {

			camera_->position_ -= camera_->matWorld_.GetYAxis() * input_->GetMouseMove().y * moveSensi_;

		}

	}

	if (input_->GetMouseWheel() != 0.0f) {

		camera_->position_+= camera_->matWorld_.GetZAxis() * float(input_->GetMouseWheel()) * moveSensi_;

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
