#include "Sprite.h"
#include <cassert>
#include "Engine/Convert.h"

#include <format>

#pragma comment(lib, "dxcompiler.lib")

ID3D12Device* Sprite::device_ = nullptr;
ID3D12GraphicsCommandList* Sprite::commandList_ = nullptr;
ID3D12RootSignature* Sprite::rootSignature_ = nullptr;
ID3D12PipelineState* Sprite::pipelineState_ = nullptr;
IDxcBlob* Sprite::vs2dBlob_ = nullptr;
IDxcBlob* Sprite::ps2dBlob_ = nullptr;

IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils,
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler,
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler)
{

	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //UTF-8の文字コードであることを通知

	LPCWSTR arguments[] = {
		filePath.c_str(), //コンパイル対象のhlslファイル名
		L"-E", L"main", //エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", //デバッグ用の情報を埋め込む
		L"-Od", //最適化を外しておく
		L"-Zpr" //メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer, //読み込んだファイル
		arguments, //コンパイルオプション
		_countof(arguments), //コンパイルオプションの数
		includeHandler.Get(), //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) //コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	//警告・エラーが出てたらログに出して止める
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告・エラーダメ
		assert(false);
	}

	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;

}

//バッファリソース作成関数
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes) {

	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; //UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; //リソースのサイズ
	//バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr;

	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));

	assert(SUCCEEDED(hr));

	return vertexResource;

}

//静的初期化
void Sprite::StaticInitialize(ID3D12Device* device, int window_width,
	int window_hwight) {

	assert(device);

	HRESULT hr;

	device_ = device;

	//dxcCompilerを初期化
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//includeに対応するための設定
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	//Shaderをコンパイルする
	vs2dBlob_ = CompileShader(L"./resources/shaders/Sprite.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vs2dBlob_ != nullptr);

	ps2dBlob_ = CompileShader(L"./resources/shaders/Sprite.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(ps2dBlob_ != nullptr);

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//D3D12_ROOT_PARAMETER rootParameters[1];
	//rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//rootParameters[0].Descriptor.ShaderRegister = 0;
	//descriptionRootSignature.pParameters = rootParameters; //ルートパラメータ配列へのポインタ
	//descriptionRootSignature.NumParameters = 1; //ルートパラメータの長さ

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ作成
	D3D12_ROOT_PARAMETER rootParameters[3]{};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	descriptionRootSignature.pParameters = rootParameters; //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); //ルートパラメータの長さ

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; //ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; //レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	//Blendstateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	/*blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;*/

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_; //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { vs2dBlob_->GetBufferPointer(),
	vs2dBlob_->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { ps2dBlob_->GetBufferPointer(),
	ps2dBlob_->GetBufferSize() }; //PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ、三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込み
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//近ければ描画
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//グラフィックスパイプラインを実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));

}

Sprite::Sprite(Texture texture, Vector2 position, Vector2 size, Vector4 color) {
	texture_ = texture;
	position_ = position;
	size_ = size;
	color_ = color;
}

Sprite* Sprite::Create(Texture texture, Vector2 position, Vector4 color) {

	Vector2 tmpSize = { 100.0f,100.0f };

	tmpSize = { static_cast<float>(texture.resource->GetDesc().Width),
	static_cast<float>(texture.resource->GetDesc().Height) };

	Sprite* sprite = new Sprite(texture, position, tmpSize, color);

	if (sprite == nullptr) {
		return nullptr;
	}

	if (!sprite->Initialize()) {
		delete sprite;
		assert(0);
		return nullptr;
	}

	return sprite;

}

bool Sprite::Initialize() {

	assert(device_);

	HRESULT hr = S_FALSE;

	//頂点バッファ
	{

		vertBuff_ = CreateBufferResource(device_, sizeof(VertexData) * 4);

		//頂点バッファビュー設定
		vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
		vbView_.SizeInBytes = sizeof(VertexData) * 4;
		vbView_.StrideInBytes = sizeof(VertexData);

		//マッピングしてデータ転送
		vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap_));

		//左下
		vertMap_[0].position = { 0.0f,size_.y, 0.0f,1.0f };
		vertMap_[0].texcoord = { 0.0f,1.0f };
		//左上
		vertMap_[1].position = { 0.0f,0.0f, 0.0f,1.0f };
		vertMap_[1].texcoord = { 0.0f,0.0f };
		//右下
		vertMap_[2].position = { size_.x,size_.y, 0.0f,1.0f };
		vertMap_[2].texcoord = { 1.0f,1.0f };
		//右上
		vertMap_[3].position = { size_.x,0.0f, 0.0f,1.0f };
		vertMap_[3].texcoord = { 1.0f,0.0f };

		//アンマップ
		vertBuff_->Unmap(0, nullptr);

	}
	//インデックスバッファ
	{

		indexBuff_ = CreateBufferResource(device_, sizeof(uint32_t) * 6);

		//インデックスバッファビュー設定
		ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
		ibView_.SizeInBytes = sizeof(uint32_t) * 6;
		ibView_.Format = DXGI_FORMAT_R32_UINT;

		//マッピングしてデータ転送
		indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap_));

		indexMap_[0] = 0;
		indexMap_[1] = 1;
		indexMap_[2] = 2;
		indexMap_[3] = 1;
		indexMap_[4] = 3;
		indexMap_[5] = 2;

		//アンマップ
		indexBuff_->Unmap(0, nullptr);

	}

	//定数バッファ
	{

		constBuff_ = CreateBufferResource(device_, sizeof(Material2D));

		//マッピングしてデータ転送
		constBuff_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));

		constMap_->color = color_;
		constMap_->uvTransform = MakeIdentity4x4();

		//アンマップ
		constBuff_->Unmap(0, nullptr);

	}

	//transformMatrix
	{

		matBuff_ = CreateBufferResource(device_, sizeof(Matrix4x4));

		matBuff_->Map(0, nullptr, reinterpret_cast<void**>(&matTransformMap_));

		*matTransformMap_ = MakeIdentity4x4();

		matBuff_->Unmap(0, nullptr);

	}

	return true;

}

void Sprite::PreDraw(ID3D12GraphicsCommandList* commandList) {

	assert(commandList_ == nullptr);

	commandList_ = commandList;

	//PSO設定
	commandList_->SetPipelineState(pipelineState_);
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_);
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Sprite::PostDraw() {
	//コマンドリスト解除
	commandList_ = nullptr;

}

void Sprite::Draw() {

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { position_.x, position_.y, 0.5f });
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	*matTransformMap_ = worldViewProjectionMatrix;

	//Spriteの描画。変更が必要なものだけ変更する
	commandList_->IASetVertexBuffers(0, 1, &vbView_);
	commandList_->SetGraphicsRootConstantBufferView(1, matBuff_->GetGPUVirtualAddress());
	//IBVを設定
	commandList_->IASetIndexBuffer(&ibView_);
	////SRVの設定
	commandList_->SetGraphicsRootDescriptorTable(2, texture_.srvHandleGPU);
	commandList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());
	//描画
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::Finalize() {

	ps2dBlob_->Release();
	vs2dBlob_->Release();
	rootSignature_->Release();
	pipelineState_->Release();

}
