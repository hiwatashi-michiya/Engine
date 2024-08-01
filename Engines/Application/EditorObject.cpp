#include "EditorObject.h"
#include "Drawing/ImGuiManager.h"
#include "UsefulFunc.h"

MapObject::MapObject() {
	model_.reset(Model::Create("./Resources/cube/cube.obj"));
	transform_ = std::make_unique<Transform>();
}

void MapObject::Initialize(const std::string& name) {

	objName_ = name;

}

void MapObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::DragFloat3("rotation", &transform_->rotate_.x, 0.01f)) {
		transform_->rotateQuaternion_ = ConvertFromEuler(transform_->rotate_);
	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {
		
	}

	{

		//vectorを中間バッファとして利用
		std::vector<char> bufferStr(tag_.begin(), tag_.end());
		//リサイズする
		bufferStr.resize(256);

		//書き換えた文字列をtagに戻す
		if (ImGui::InputText("tag", bufferStr.data(), bufferStr.size())) {
			auto endIt = std::find(bufferStr.begin(), bufferStr.end(), '\0');
			tag_ = std::string(bufferStr.begin(), endIt);
		}

	}

#endif // _DEBUG

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void MapObject::Draw(Camera* camera) {

	model_->Draw(camera);

}

void PlayerObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/human/walking.gltf");
	objName_ = name;
	tag_ = "player";

}

void PlayerObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

#endif // _DEBUG

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void BlockObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/block/block.obj");
	model_->SetColor(CreateColor(colorNumber_));
	objName_ = name;
	tag_ = "block";

}

void BlockObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {

	}

#endif // _DEBUG

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void MoveBoxObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/block/block.obj");
	model_->SetTexture("./Resources/block/circle.png");
	model_->SetColor(CreateColor(colorNumber_));
	objName_ = name;
	tag_ = "moveBox";

}

void MoveBoxObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {

	}

	if (ImGui::SliderInt("color", &colorNumber_, 0, kMaxColor_ - 1)) {
		model_->SetColor(CreateColor(colorNumber_));
	}

#endif // _DEBUG

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void RingObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/item/item.gltf");
	model_->SetColor(CreateColor(colorNumber_));
	objName_ = name;
	tag_ = "ring";

}

void RingObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::SliderInt("color", &colorNumber_, 0, kMaxColor_ - 1)) {
		model_->SetColor(CreateColor(colorNumber_));
	}

#endif // _DEBUG

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void GoalObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/goal/goal.obj");
	model_->SetColor(CreateColor(colorNumber_));
	objName_ = name;
	tag_ = "goal";

}

void GoalObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

#endif // _DEBUG

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void WarpObject::Initialize(const std::string& name) {

	modelB_.reset(Model::Create("./Resources/warp/warp.obj"));
	transformB_ = std::make_unique<Transform>();
	transformB_->translate_ = transform_->translate_ + Vector3{ 10.0f,0.0f,0.0f };
	model_->SetMesh("./Resources/warp/warp.obj");
	objName_ = name;
	tag_ = "warp";

	model_->SetColor(CreateColor(colorNumber_));
	modelB_->SetColor(CreateColor(colorNumber_));

}

void WarpObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("positionA", &transform_->translate_.x, 0.1f)) {
		isMoveA_ = true;
	}

	if (ImGui::DragFloat3("positionB", &transformB_->translate_.x, 0.1f)) {
		isMoveA_ = false;
	}

	if (ImGui::SliderInt("color", &colorNumber_, 0, kMaxColor_ - 1)) {
		model_->SetColor(CreateColor(colorNumber_));
		modelB_->SetColor(CreateColor(colorNumber_));
	}

	//どちらを動かすか選択
	ImGui::Checkbox("move A", &isMoveA_);

#endif // _DEBUG

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);
	transformB_->UpdateMatrix();
	model_->SetWorldMatrix(transformB_->worldMatrix_);

}

void WarpObject::Draw(Camera* camera) {

	model_->Draw(camera);
	modelB_->Draw(camera);

}

void GhostBoxObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/block/block.obj");
	model_->SetColor(CreateColor(colorNumber_));
	objName_ = name;
	tag_ = "ghostBox";

}

void GhostBoxObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {

	}

	if (ImGui::SliderInt("color", &colorNumber_, 0, kMaxColor_ - 1)) {
		model_->SetColor(CreateColor(colorNumber_));
	}

#endif // _DEBUG

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void SwitchObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/switch/switch.obj");
	model_->SetColor(CreateColor(colorNumber_));
	objName_ = name;
	tag_ = "switch";

}

void SwitchObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::SliderInt("color", &colorNumber_, 0, kMaxColor_ - 1)) {
		model_->SetColor(CreateColor(colorNumber_));
	}

#endif // _DEBUG

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}
