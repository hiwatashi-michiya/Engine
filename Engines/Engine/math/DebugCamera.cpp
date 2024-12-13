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
	camera_->position_ = startPosition_;
	camera_->rotation_.x = startRotate_;

}

void DebugCamera::Update() {

	//マウスホイールをクリックした時
	if (input_->PushMouse(Input::Mouse::kCenter)) {

		//マウスの動きに合わせて回転
		if (input_->GetMouseMove().x != 0.0f) {

			camera_->rotation_.y += input_->GetMouseMove().x * rotateSensi_;

		}

		if (input_->GetMouseMove().y != 0.0f) {

			camera_->rotation_.x += input_->GetMouseMove().y * rotateSensi_;

		}

	}
	//右クリックした時
	else if (input_->PushMouse(Input::Mouse::kRight)) {

		//マウスの動きに合わせて平行移動
		if (input_->GetMouseMove().x != 0.0f) {

			camera_->position_ -= camera_->matWorld_.GetXAxis() * input_->GetMouseMove().x * moveSensi_;

		}

		if (input_->GetMouseMove().y != 0.0f) {

			camera_->position_ -= camera_->matWorld_.GetYAxis() * input_->GetMouseMove().y * moveSensi_;

		}

	}
	//マウスホイールを回した時
	if (input_->GetMouseWheel() != 0.0f) {
		//カメラのZ軸を平行移動
		camera_->position_+= camera_->matWorld_.GetZAxis() * float(input_->GetMouseWheel()) * moveSensi_;

	}

	//ここからカメラの更新
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
