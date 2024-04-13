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

	editor_ = MapEditor::GetInstance();
	editor_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,44.0f, -33.0f };
	camera_->rotation_.x = 0.9f;

	editor_->SetCamera(camera_.get());

	model_.reset(Model::Create("./Resources/AnimatedCube/AnimatedCube.gltf"));
	animation_ = std::make_unique<Animation>();
	*animation_ = LoadAnimationFile("./Resources/AnimatedCube/AnimatedCube.gltf");

}

void SelectScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();


#endif // _DEBUG

	animationTime_ += 1.0f / 60.0f;
	animationTime_ = std::fmod(animationTime_, animation_->duration);
	NodeAnimation& rootNodeAnimation = animation_->nodeAnimations[model_->mesh_->modelData_.rootNode.name]; //rootNodeのanimationを取得
	Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyFrames, animationTime_);
	Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyFrames, animationTime_);
	Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyFrames, animationTime_);
	localMatrix_ = MakeAffineMatrix(scale, rotate, translate);

	editor_->Edit();

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

}

void SelectScene::Draw() {

	editor_->Draw(camera_.get());

	model_->Draw(localMatrix_, camera_.get());

}
