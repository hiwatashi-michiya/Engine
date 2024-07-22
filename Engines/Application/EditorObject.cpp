#include "EditorObject.h"
#include "Drawing/ImGuiManager.h"

MapObject::MapObject() {
	model_.reset(Model::Create("./Resources/cube/cube.obj"));
	transform_ = std::make_unique<Transform>();
}

void MapObject::Initialize(const std::string& name) {

	objName_ = name;

}

void MapObject::Edit() {

#ifdef _DEBUG

	isOpen_ = true;

	if (isOpen_ && preOpen_) {
		
	}

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

	isOpen_ = true;

	if (isOpen_ && preOpen_) {

	}

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void BlockObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/block/block.obj");
	objName_ = name;
	tag_ = "block";

}

void BlockObject::Edit() {

	isOpen_ = true;

	if (isOpen_ && preOpen_) {

	}

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {

	}

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void MoveBoxObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/block/block.obj");
	objName_ = name;
	tag_ = "moveBox";

}

void MoveBoxObject::Edit() {

	isOpen_ = true;

	if (isOpen_ && preOpen_) {

	}

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {

	}

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void RingObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/item/item.gltf");
	objName_ = name;
	tag_ = "item";

}

void RingObject::Edit() {

	isOpen_ = true;

	if (isOpen_ && preOpen_) {

	}

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void GoalObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/goal/goal.obj");
	objName_ = name;
	tag_ = "goal";

}

void GoalObject::Edit() {

	isOpen_ = true;

	if (isOpen_ && preOpen_) {

	}

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void WarpObject::Initialize(const std::string& name) {

	modelB_.reset(Model::Create("./Resources/block/block.obj"));
	transformB_ = std::make_unique<Transform>();
	transformB_->translate_ = transform_->translate_ + Vector3{ 10.0f,0.0f,0.0f };

	model_->SetMesh("./Resources/block/block.obj");
	objName_ = name;
	tag_ = "warp";

}

void WarpObject::Edit() {

	isOpen_ = true;

	if (isOpen_ && preOpen_) {

	}

	if (ImGui::DragFloat3("positionA", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::DragFloat3("positionB", &transformB_->translate_.x, 0.1f)) {

	}

	transform_->UpdateMatrix();
	model_->SetWorldMatrix(transform_->worldMatrix_);
	transformB_->UpdateMatrix();
	model_->SetWorldMatrix(transformB_->worldMatrix_);

}

void WarpObject::Draw(Camera* camera) {

	model_->Draw(camera);
	modelB_->Draw(camera);

}
