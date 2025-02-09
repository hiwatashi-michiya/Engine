#include "InstancingModel.h"
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
#include "Core/DirectXSetter.h"

#pragma comment(lib, "dxcompiler.lib")

InstancingModel* InstancingModel::Create(const std::string& filename) {

	InstancingModel* model = new InstancingModel();

	model->Initialize(filename);

	return model;

}

void InstancingModel::Initialize(const std::string& filename) {

	assert(DirectXSetter::GetInstance()->GetDevice());

	if (MeshManager::GetInstance()->IsExistMesh(filename)) {

		mesh_ = MeshManager::GetInstance()->GetMesh(filename);

	}
	else {

		//メッシュを登録
		MeshManager::GetInstance()->CreateMesh(filename);
		mesh_ = MeshManager::GetInstance()->GetMesh(filename);

	}

	material_ = std::make_unique<Material>();
	material_->Create();

	texture_ = TextureManager::GetInstance()->Load(mesh_->textureFilePath_);
	maskTexture_ = TextureManager::GetInstance()->Load("./Resources/EngineResources/defaultMask.png");

	//transformMatrix
	{

		matBuff_ = CreateBufferResource(DirectXSetter::GetInstance()->GetDevice(), sizeof(InstancingForGPU) * kMaxInstance_);

		matBuff_->SetName(L"matrixBuff");

		matBuff_->Map(0, nullptr, reinterpret_cast<void**>(&matTransformMap_));

		//インスタンシングの数だけループ
		for (uint32_t i = 0; i < kMaxInstance_; i++) {
			matTransformMap_[i].WVP = MakeIdentity4x4();
			matTransformMap_[i].World = MakeIdentity4x4();
			matTransformMap_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		}

		matBuff_->Unmap(0, nullptr);

	}

	//カメラ設定
	{

		cameraBuff_ = CreateBufferResource(DirectXSetter::GetInstance()->GetDevice(), sizeof(CameraForGPU));

		cameraBuff_->SetName(L"cameraBuff");

		cameraBuff_->Map(0, nullptr, reinterpret_cast<void**>(&cameraMap_));

		cameraMap_->worldPosition = Vector3::Zero();

		cameraBuff_->Unmap(0, nullptr);

	}

	//インスタンシングリソース設定
	{

		instancingResource_ = TextureManager::GetInstance()->SetInstancingResource(kMaxInstance_, matBuff_);

	}

}


void InstancingModel::Render(ID3D12GraphicsCommandList* commandList)
{

	//カメラ設定
	commandList->SetGraphicsRootConstantBufferView(4, cameraBuff_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matBuff_->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);
	commandList->SetGraphicsRootDescriptorTable(6, maskTexture_->srvHandleGPU);
	commandList->SetGraphicsRootDescriptorTable(7, instancingResource_.srvHandleGPU);

	//描画
	material_->SetCommandMaterial(commandList);

	mesh_->SetCommandMesh(commandList, instanceCount_);
	//インスタンスカウントリセット
	instanceCount_ = 0;

}

void InstancingModel::SetTexture(const std::string& name) {

	texture_ = TextureManager::GetInstance()->Load(name);

}

void InstancingModel::ImGuiUpdate(const std::string& name) {

#ifdef _DEBUG

	ImGui::Begin(name.c_str());

	if (ImGui::BeginTabBar("InstancingModel", ImGuiTabBarFlags_None)) {

		if (ImGui::BeginTabItem("material")) {
			material_->ImGuiUpdate();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

#endif // _DEBUG

}

void InstancingModel::AddModelData(Model* model)
{
	//最大数を超えていたら追加しない
	if (instanceCount_ >= kMaxInstance_) {
		return;
	}

	matTransformMap_[instanceCount_].WVP = model->worldViewProjectionMatrix_;
	matTransformMap_[instanceCount_].World = model->localMatrix_ * model->worldMatrix_;
	matTransformMap_[instanceCount_].WorldInverseTranspose = Transpose(Inverse(model->localMatrix_ * model->worldMatrix_));
	matTransformMap_[instanceCount_].color = model->color_;

	AddInstanceCount();

}

void InstancingModel::SetCamera(Camera* camera)
{

	cameraMap_->worldPosition = camera->GetWorldPosition();

}
