#include "TextureManager.h"
#include "Engine/base/DirectXSetter.h"
#include "externals/DirectXTex/DirectXTex.h"
#include"Engine/Convert.h"
#include "Engine/math/Matrix4x4.h"
#include "DescriptorHeap/DescriptorHeapManager.h"

//DirectXTexを使ってTextureを読み込むためのLoadTexture関数
DirectX::ScratchImage LoadTexture(const std::string& filePath) {

	//テクスチャファイルを読んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;

}

//読み込んだTexture情報を基にTextureResourceを作る関数
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
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
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; //細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; //WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //プロセッサの近くに配置
	//3.Resourceを生成する
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr;

	hr = device->CreateCommittedResource(
		&heapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ, //初回のResourceState。Textureは基本読むだけ
		nullptr, //Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)); //作成するResourceポインタへのポインタ

	assert(SUCCEEDED(hr));

	return resource;

}

//データを転送するUploadTextureData関数
void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {

	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		//MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr;

		hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr, //全領域へコピー
			img->pixels, //元データアドレス
			UINT(img->rowPitch), //1ラインサイズ
			UINT(img->slicePitch) //1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}

}

//CPUのDescriptorHandle取得
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;

}

//GPUのDescriptorHandle取得
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;

}

//デスクリプタヒープ作成関数
//ID3D12DescriptorHeap* CreateDescriptorHeap(
//	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
//
//	//ディスクリプタヒープの生成
//	ID3D12DescriptorHeap* descriptorHeap = nullptr;
//	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
//	descriptorHeapDesc.Type = heapType;
//	descriptorHeapDesc.NumDescriptors = numDescriptors;
//	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	HRESULT hr;
//
//	hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
//
//	//ディスクリプタヒープが作れなかったので起動できない
//	assert(SUCCEEDED(hr));
//
//	return descriptorHeap;
//
//}

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize() {

	ID3D12Device* device = DirectXSetter::GetInstance()->GetDevice();
	device_ = device;

	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvDescHeap_ = DescriptorHeapManager::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kNumDescriptors, true, "SRVHeap");

	srvDescHeap_->SetName(L"srvHeap");

	for (size_t i = 0; i < kNumDescriptors; i++) {
		textures_[i].resource.Reset();
		textures_[i].srvHandleCPU.ptr = 0;
		textures_[i].srvHandleGPU.ptr = 0;
	}

}

Texture* TextureManager::Load(const std::string& filePath) {

	if (textureMap_.find(filePath) != textureMap_.end()) {

		return textureMap_[filePath].get();

	}

	//制限数以上の読み込みで止める
	assert(textureIndex_ < kMaxTextures - 1);

	std::unique_ptr<Texture> tex = std::make_unique<Texture>();

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	tex->resource = CreateTextureResource(device_, metadata);
	UploadTextureData(tex->resource, mipImages);

	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	tex->srvHandleCPU = GetCPUDescriptorHandle(srvDescHeap_, descriptorSizeSRV, 1 + textureIndex_);
	tex->srvHandleGPU = GetGPUDescriptorHandle(srvDescHeap_, descriptorSizeSRV, 1 + textureIndex_);

	//SRVの生成
	device_->CreateShaderResourceView(tex->resource.Get(), &srvDesc, tex->srvHandleCPU);

	textureMap_[filePath] = std::move(tex);

	//使用カウント上昇
	textureIndex_++;

	return textureMap_[filePath].get();

}

InstancingResource TextureManager::SetInstancingResource(uint32_t instanceCount, Microsoft::WRL::ComPtr<ID3D12Resource> mapResource) {

	//制限数以上の読み込みで止める
	assert(textureIndex_ < kMaxTextures - 1);

	InstancingResource instancingResource;

	instancingResource.resource = mapResource;

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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
	instancingResource.srvHandleCPU = GetCPUDescriptorHandle(srvDescHeap_, descriptorSizeSRV, 1 + textureIndex_);
	instancingResource.srvHandleGPU = GetGPUDescriptorHandle(srvDescHeap_, descriptorSizeSRV, 1 + textureIndex_);

	//SRVの生成
	device_->CreateShaderResourceView(instancingResource.resource.Get(), &srvDesc, instancingResource.srvHandleCPU);

	//使用カウント上昇
	textureIndex_++;

	return instancingResource;

}

void TextureManager::Finalize() {

	textureMap_.clear();

}
