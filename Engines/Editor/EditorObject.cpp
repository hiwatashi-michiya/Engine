#include "EditorObject.h"
#include "Drawing/ImGuiManager.h"
#include "UniqueEditor.h"

std::stack<std::shared_ptr<ICommand>>& MapObject::undoCommands_ = UniqueEditor::GetInstance()->GetUndoCommands();
std::stack<std::shared_ptr<ICommand>>& MapObject::redoCommands_ = UniqueEditor::GetInstance()->GetRedoCommands();

MapObject::MapObject() {
	model_ = std::make_unique<Model>();
	model_->Initialize("./Resources/block/block.obj");
	transform_ = std::make_unique<Transform>();
	oldTransform_ = std::make_unique<Transform>();
	obb_ = std::make_unique<OBB>();
	lineBox_ = std::make_unique<LineBox>();
	lineBox_->SetIsCollisionLine(false);
	lineBox_->SetOBB(obb_.get());
}

void MapObject::Initialize(const std::string& name) {

	objName_ = name;

}

void MapObject::Update()
{

	transform_->UpdateMatrix();

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void MapObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {
		isUsing_ = true;
	}


	if (ImGui::DragFloat3("rotation", &transform_->rotate_.x, 0.01f)) {
		transform_->rotateQuaternion_ = ConvertFromEuler(transform_->rotate_);
		isUsing_ = true;
	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {
		isUsing_ = true;
	}

	RecordMove();

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

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void MapObject::RecordMove()
{

#ifdef _DEBUG

	//動かした瞬間に古い値を記録
	if (isUsing_ and not preIsUsing_) {

		oldTransform_->scale_ = transform_->worldMatrix_.GetScale();
		oldTransform_->rotateQuaternion_ = ConvertFromRotateMatrix(transform_->worldMatrix_.GetRotateMatrix());
		oldTransform_->translate_ = transform_->worldMatrix_.GetTranslate();

	}

	//動かし終わった瞬間にコマンド追加
	if (isUsing_ and not ImGui::IsAnyItemActive()) {

		isUsing_ = false;
		std::shared_ptr<MoveCommand> newMoveCommand =
			std::make_shared<MoveCommand>(*transform_,
				*oldTransform_, *transform_);
		undoCommands_.push(newMoveCommand);
		
		while (not redoCommands_.empty())
		{
			redoCommands_.pop();
		}

	}

	preIsUsing_ = isUsing_;

#endif // _DEBUG

}

void MapObject::Draw(Camera* camera) {

	model_->Draw(camera);
	lineBox_->Draw(camera);

}


void PlayerObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/player/brush_walk.gltf");
	transform_->rotateQuaternion_ = ConvertFromEuler(Vector3{ -0.0f, 0.0f, 0.0f });
	objName_ = name;
	tag_ = "player";

}

void PlayerObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {
		isUsing_ = true;
	}

	RecordMove();

#endif // _DEBUG

	transform_->UpdateMatrix();

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void BlockObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/block/block.obj");
	objName_ = name;
	tag_ = "block";

}

void BlockObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {
		isUsing_ = true;
	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {
		isUsing_ = true;
	}

	RecordMove();

#endif // _DEBUG

	model_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	transform_->UpdateMatrix();

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void GhostBoxObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/ghostBox/ghostBox.obj");
	model_->SetColor(CreateColor(color_));
	objName_ = name;
	tag_ = "ghostBox";

}

void GhostBoxObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {
		isUsing_ = true;
	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {
		isUsing_ = true;
	}

	RecordMove();

#endif // _DEBUG

	transform_->UpdateMatrix();

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void SwitchObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/bucket/bucket.obj");
	model_->SetColor(CreateColor(color_));
	objName_ = name;
	tag_ = "switch";

}

void SwitchObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {
		isUsing_ = true;
	}

	if (ImGui::DragFloat3("scale", &transform_->scale_.x, 0.01f)) {
		isUsing_ = true;
	}

	if (ImGui::Combo("color", reinterpret_cast<int*>(&color_), GameColor::colorNames.data(), int(GameColor::colorNames.size()))) {
		model_->SetColor(GameColor::CreateColor(color_));
	}

	RecordMove();

#endif // _DEBUG

	transform_->UpdateMatrix();

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void SpriteFlagObject::Initialize(const std::string& name)
{

	model_->SetMesh("./Resources/bucket/bucket.obj");
	objName_ = name;
	tag_ = "spriteFlag";

}

void SpriteFlagObject::Edit()
{

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {
		isUsing_ = true;
	}

	RecordMove();

#endif // _DEBUG

	transform_->UpdateMatrix();

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}
