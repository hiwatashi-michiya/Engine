#include "Model.h"
#include <cassert>
#include "Engine/Convert.h"
#include "Engine/Drawing/ShaderManager.h"
#include <fstream>
#include <sstream>
#include "Drawing/ImGuiManager.h"
#include "Drawing/RootSignatureManager.h"
#include "Buffer/BufferResource.h"
#include "Drawing/PipelineManager.h"
#include "Drawing/MeshManager.h"
#include "Core/RootSignatureDesc.h"
#include "Core/RootParameter.h"
#include "Core/StaticSampler.h"
#include "Core/InputElement.h"
#include "Core/InputLayout.h"
#include "Core/DescriptorRange.h"
#include "Drawing/RenderManager.h"
#include "ModelManager.h"
#include "InstancingModel.h"

#pragma comment(lib, "dxcompiler.lib")

void Model::Initialize(const std::string& filename, [[maybe_unused]] const std::string& texturename) {

	localMatrix_ = Matrix4x4::Identity();
	worldMatrix_ = Matrix4x4::Identity();
	worldViewProjectionMatrix_ = Matrix4x4::Identity();
	color_ = { 1.0f,1.0f,1.0f,1.0f };
	//既にインスタンシング用のモデルを作成している場合、それを返す
	if (ModelManager::GetInstance()->IsExistModel(filename)) {
		instancingModel_ = ModelManager::GetInstance()->GetModel(filename);
		return;
	}
	//新規でインスタンシング用のモデルを作成
	ModelManager::GetInstance()->AddModel(filename);
	//ポインタを渡す
	instancingModel_ = ModelManager::GetInstance()->GetModel(filename);

}

void Model::LoadAnimation(const std::string& filename) {

	//インスタンス生成
	if (!animation_) {

		animation_ = std::make_unique<Animation>();

	}

	*animation_ = LoadAnimationFile(filename);

	if (!skeleton_) {

		skeleton_ = std::make_unique<Skeleton>();

	}

	*skeleton_ = CreateSkeleton(instancingModel_->mesh_->modelData_.rootNode);

}

void Model::ResetAnimation() {

	//アニメーションが存在している時のみリセットを行う
	if (animation_ and animation_->nodeAnimations.size() != 0) {

		animationTime_ = 0.0f;
		NodeAnimation& rootNodeAnimation = animation_->nodeAnimations[instancingModel_->mesh_->modelData_.rootNode.name]; //rootNodeのanimationを取得
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyFrames, animationTime_);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyFrames, animationTime_);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyFrames, animationTime_);
		localMatrix_ = MakeAffineMatrix(scale, rotate, translate);

	}

}

void Model::UpdateAnimation() {

	isEndAnimation_ = false;

	//アニメーションが存在していて、再生フラグが立っている時
	if (animation_ and isStartAnimation_ and animation_->nodeAnimations.size() != 0) {

		//現在のアニメーションタイムをアニメーション速度分加算
		animationTime_ += animationSpeed_ / 60.0f;

		//アニメーションタイムが全体の尺を超えていたら終点とみなす
		if (animationTime_ >= animation_->duration) {
			
			animationTime_ = animation_->duration;

			//ループしなければフラグを降ろす
			if (!isLoop_) {
				isStartAnimation_ = false;
			}

			isEndAnimation_ = true;
		}

		//アニメーションの時間調整
		animationTime_ = std::fmod(animationTime_, animation_->duration);
		
		if (skeleton_) {

			ApplyAnimation(*skeleton_, *animation_, animationTime_);
			UpdateSkeleton(*skeleton_);

		}

		NodeAnimation& rootNodeAnimation = animation_->nodeAnimations[instancingModel_->mesh_->modelData_.rootNode.name]; //rootNodeのanimationを取得
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyFrames, animationTime_);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyFrames, animationTime_);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyFrames, animationTime_);
		localMatrix_ = MakeAffineMatrix(scale, rotate, translate);

	}

}

void Model::Draw(Camera* camera) {

	//ワールドビュープロジェクション更新
	worldViewProjectionMatrix_ = localMatrix_ * worldMatrix_ * camera->matViewProjection_;
	//カメラセット
	instancingModel_->SetCamera(camera);
	//モデルのデータを追加
	instancingModel_->AddModelData(this);

}

void Model::SetMesh(const std::string& filename, [[maybe_unused]] const std::string& texturename)
{

	//既にインスタンシング用のモデルを作成している場合、それを返す
	if (ModelManager::GetInstance()->IsExistModel(filename)) {
		instancingModel_ = ModelManager::GetInstance()->GetModel(filename);
		return;
	}
	//新規でインスタンシング用のモデルを作成
	ModelManager::GetInstance()->AddModel(filename);
	//ポインタを渡す
	instancingModel_ = ModelManager::GetInstance()->GetModel(filename);

}

