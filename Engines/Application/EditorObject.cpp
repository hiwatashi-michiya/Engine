#include "EditorObject.h"
#include "Drawing/ImGuiManager.h"
#include "UsefulFunc.h"

MapObject::MapObject() {
	model_.reset(Model::Create("./Resources/cube/cube.obj"));
	transform_ = std::make_unique<Transform>();
	obb_ = std::make_unique<OBB>();
	lineBox_ = std::make_unique<LineBox>();
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

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void MapObject::Draw(Camera* camera) {

	model_->Draw(camera);

}

void MapObject::DrawLine(Camera* camera)
{

	lineBox_->Draw(camera);

}

void PlayerObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/human/walking.gltf");
	transform_->rotateQuaternion_ = ConvertFromEuler(Vector3{ -1.57f, 0.0f, 0.0f });
	objName_ = name;
	tag_ = "player";

}

void PlayerObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

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

void BlockObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/block/block.obj");
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

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

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

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

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

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

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

	obbB_ = std::make_unique<OBB>();
	lineBoxB_ = std::make_unique<LineBox>();
	lineBoxB_->SetOBB(obbB_.get());

}

void WarpObject::Update()
{

	transform_->UpdateMatrix();

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

	transformB_->UpdateMatrix();

	obbB_->center = transformB_->translate_;
	obbB_->size = transformB_->scale_;
	obbB_->orientations[0] = Normalize(transformB_->worldMatrix_.GetXAxis());
	obbB_->orientations[1] = Normalize(transformB_->worldMatrix_.GetYAxis());
	obbB_->orientations[2] = Normalize(transformB_->worldMatrix_.GetZAxis());

	lineBoxB_->Update();

	modelB_->SetWorldMatrix(transformB_->worldMatrix_);

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

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);
	transformB_->UpdateMatrix();
	model_->SetWorldMatrix(transformB_->worldMatrix_);

}

void WarpObject::Draw(Camera* camera) {

	model_->Draw(camera);
	modelB_->Draw(camera);

}

void WarpObject::DrawLine(Camera* camera)
{

	lineBox_->Draw(camera);
	lineBoxB_->Draw(camera);

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

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

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

	obb_->center = transform_->translate_;
	obb_->size = transform_->scale_;
	obb_->orientations[0] = Normalize(transform_->worldMatrix_.GetXAxis());
	obb_->orientations[1] = Normalize(transform_->worldMatrix_.GetYAxis());
	obb_->orientations[2] = Normalize(transform_->worldMatrix_.GetZAxis());

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

}

void CopyCatObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/human/walking.gltf");
	transform_->rotateQuaternion_ = ConvertFromEuler(Vector3{ -1.57f, 0.0f, 0.0f });
	objName_ = name;
	tag_ = "copyCat";

}

void CopyCatObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::SliderInt("color", &colorNumber_, -1, kMaxColor_ - 1)) {
		model_->SetColor(CreateColor(colorNumber_));
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

void EnemyObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/enemy/enemy.obj");
	model_->SetColor(CreateColor(colorNumber_));
	objName_ = name;
	tag_ = "enemy";

}

void EnemyObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

	}

	if (ImGui::SliderInt("color", &colorNumber_, 0, kMaxColor_ - 1)) {
		model_->SetColor(CreateColor(colorNumber_));
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

void HolderObject::Initialize(const std::string& name) {

	model_->SetMesh("./Resources/colorHolder/colorHolder.obj");
	modelB_.reset(Model::Create("./Resources/colorHolder/colorHolder_inner.obj"));
	modelB_->SetWorldMatrix(transform_->worldMatrix_);
	objName_ = name;
	tag_ = "colorHolder";
	colorNumber_ = -1;

}

void HolderObject::Edit() {

#ifdef _DEBUG

	if (ImGui::DragFloat3("position", &transform_->translate_.x, 0.1f)) {

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
	modelB_->SetWorldMatrix(transform_->worldMatrix_);

}

void HolderObject::Draw(Camera* camera) {

	model_->Draw(camera);
	modelB_->Draw(camera);

}
