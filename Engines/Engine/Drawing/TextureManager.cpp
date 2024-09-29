#include "TextureManager.h"
#include "Core/DirectXSetter.h"
#include "externals/DirectXTex/DirectXTex.h"
#include"Engine/Convert.h"
#include "Engine/math/Matrix4x4.h"
#include "DescriptorHeap/DescriptorHeapManager.h"
#include "Externals/DirectXTex/d3dx12.h"
#include "Buffer/BufferResource.h"
#include "Core/DescriptorHandle.h"

//DirectXTexを使ってTextureを読み込むためのLoadTexture関数
DirectX::ScratchImage LoadTexture(const std::string& filePath) {

	//テクスチャファイルを読んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr;

	//.ddsで終わっていたらddsとみなす
	if (filePathW.ends_with(L".dds")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}

	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};

	//圧縮フォーマットかどうかを調べる
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		//圧縮フォーマットならそのまま使うのでmoveする
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	}
	
	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;

}

//読み込んだTexture情報を基にTextureResourceを作る関数
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata) {
	//1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); //Textureの幅
	resourceDesc.Height = UINT(metadata.height); //Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); //mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); //奥行きor配列Textureの配列数
	resourceDesc.Format = metadata.format; //TextureのFormat
	resourceDesc.SampleDesc.Count = 1; //サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); //Textureの次元数。普段使っているのは二次元
	//2.利用するHeapの設定。特殊な運用
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //defaultに設定
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; //WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //プロセッサの近くに配置

	//3.Resourceを生成する
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr;

	hr = device->CreateCommittedResource(
		&heapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, //データ転送される設定
		nullptr, //Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)); //作成するResourceポインタへのポインタ

	assert(SUCCEEDED(hr));

	return resource;

}

//データを転送するUploadTextureData関数
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages,
	Microsoft::WRL::ComPtr<ID3D12Device> device, ID3D12GraphicsCommandList* commandList) {

	//中間リソースの作成
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
	DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subResources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subResources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
	
	//データ転送をコマンドに積む
	UpdateSubresources(commandList, texture.Get(), intermediateResource.Get(), 0, 0, UINT(subResources.size()), subResources.data());

	//ResourceStateを変更し、IntermediateResourceを返す

	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;

}

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap) {

	device_ = DirectXSetter::GetInstance()->GetDevice();

	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvDescHeap_ = srvHeap;

	for (size_t i = 0; i < kNumDescriptors; i++) {
		textures_[i].resource.Reset();
		textures_[i].srvHandleCPU.ptr = 0;
		textures_[i].srvHandleGPU.ptr = 0;
	}

	textureIndex_ = 0;

}

Texture* TextureManager::Load(const std::string& filePath) {

	std::string path = filePath;

	if (path.empty()) {
		path = "./Resources/white.png";
	}


	if (textureMap_.find(path) != textureMap_.end()) {

		return textureMap_[path].get();

	}

	//制限数以上の読み込みで止める
	assert(DirectXSetter::srvHandleNumber_ < kMaxTextures);

	std::unique_ptr<Texture> tex = std::make_unique<Texture>();

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture(path);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	tex->resource = CreateTextureResource(device_, metadata);
	intermediateResources_[DirectXSetter::srvHandleNumber_] = UploadTextureData(tex->resource, mipImages, device_, DirectXSetter::GetInstance()->GetCommandList());

	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	
	if (metadata.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0; //unionがTextureCubeになったが、内部パラメータの意味はTexture2dと変わらない
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	}

	//SRVを作成するDescriptorHeapの場所を決める
	tex->srvHandleCPU = GetCPUDescriptorHandle(srvDescHeap_, descriptorSizeSRV_, DirectXSetter::srvHandleNumber_);
	tex->srvHandleGPU = GetGPUDescriptorHandle(srvDescHeap_, descriptorSizeSRV_, DirectXSetter::srvHandleNumber_);

	//SRVの生成
	device_->CreateShaderResourceView(tex->resource.Get(), &srvDesc, tex->srvHandleCPU);

	textureMap_[path] = std::move(tex);

	//使用カウント上昇
	DirectXSetter::srvHandleNumber_++;

	return textureMap_[path].get();

}

InstancingResource TextureManager::SetInstancingResource(uint32_t instanceCount, Microsoft::WRL::ComPtr<ID3D12Resource> mapResource) {

	//制限数以上の読み込みで止める
	assert(DirectXSetter::srvHandleNumber_ < kMaxTextures);

	InstancingResource instancingResource;

	instancingResource.resource = mapResource;

	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = instanceCount;
	srvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);

	//SRVを作成するDescriptorHeapの場所を決める
	instancingResource.srvHandleCPU = GetCPUDescriptorHandle(srvDescHeap_, descriptorSizeSRV_, DirectXSetter::srvHandleNumber_);
	instancingResource.srvHandleGPU = GetGPUDescriptorHandle(srvDescHeap_, descriptorSizeSRV_, DirectXSetter::srvHandleNumber_);

	//SRVの生成
	device_->CreateShaderResourceView(instancingResource.resource.Get(), &srvDesc, instancingResource.srvHandleCPU);

	//使用カウント上昇
	DirectXSetter::srvHandleNumber_++;

	return instancingResource;

}

void TextureManager::Finalize() {

	textureMap_.clear();

}
