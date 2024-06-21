#include "SkinCluster.h"
#include "Buffer/BufferResource.h"
#include "Drawing/TextureManager.h"
#include "base/DescriptorHandle.h"
#include <cassert>
#include "base/DirectXSetter.h"

SkinCluster::SkinCluster()
{
}

SkinCluster::~SkinCluster()
{
}

void SkinCluster::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton,
	const ModelData& modelData) {

	uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	

	//palette用のResourceを確保
	paletteResource_ = CreateBufferResource(device, sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	paletteResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	mappedPalette_ = { mappedPalette, skeleton.joints.size() }; //spanを使ってアクセスするようにする
	paletteSrvHandle_.first = GetCPUDescriptorHandle(DirectXSetter::GetInstance()->GetSrvHeap(), descriptorSizeSRV, DirectXSetter::srvHandleNumber_);
	paletteSrvHandle_.second = GetGPUDescriptorHandle(DirectXSetter::GetInstance()->GetSrvHeap(), descriptorSizeSRV, DirectXSetter::srvHandleNumber_);
	paletteResource_->Unmap(0, nullptr);

	DirectXSetter::srvHandleNumber_++;

	//palette用のsrvを作成。StructuredBufferでアクセスできるようにする
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	device->CreateShaderResourceView(paletteResource_.Get(), &paletteSrvDesc, paletteSrvHandle_.first);

	//influence用のResourceを確保。頂点毎にinfluence情報を追加できるようにする
	influenceResource_ = CreateBufferResource(device, sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	influenceResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size()); //0埋め。weightを0にしておく。
	mappedInfluence_ = { mappedInfluence, modelData.vertices.size() };
	influenceResource_->Unmap(0, nullptr);

	//Influence用のVBVを作成
	influenceBufferView_.BufferLocation = influenceResource_->GetGPUVirtualAddress();
	influenceBufferView_.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	influenceBufferView_.StrideInBytes = sizeof(VertexInfluence);

	//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
	inverseBindPoseMatrices_.resize(skeleton.joints.size());
	std::generate(inverseBindPoseMatrices_.begin(), inverseBindPoseMatrices_.end(), MakeIdentity4x4);

	//ModelDataを解析してInfluenceを埋める

	//ModelのSkinClusterの情報を解析
	for (const auto& jointWeight : modelData.skinClusterData) {
		
		//skeletonに対象となるJointが含まれているか判断
		auto it = skeleton.jointMap.find(jointWeight.first);
		//存在しなかったら次に回す
		if (it == skeleton.jointMap.end()) {
			continue;
		}

		//該当のindexのinverseBindPoseMatrixを代入
		inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix;

		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {

			//該当のvertexIndexのinfluence情報を参照しておく
			auto& currentInfluence = mappedInfluence_[vertexWeight.vertexIndex];
			//空いている所に入れる
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {

				//weight == 0.0fが空いている状態なので、その場所にweightとjointのindexを代入
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}

			}

		}


	}

	//テクスチャ使用カウント上昇
	TextureManager::GetInstance()->AddIndex();

}

void SkinCluster::Update(const Skeleton& skeleton) {

	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {

		assert(jointIndex < inverseBindPoseMatrices_.size());

		mappedPalette_[jointIndex].skeletonSpaceMatrix =
			inverseBindPoseMatrices_[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;

		mappedPalette_[jointIndex].skeletonSpaceInverseTransposeMatrix =
			Transpose(Inverse(mappedPalette_[jointIndex].skeletonSpaceMatrix));

	}

}
