#include "Particle3D.h"
#include <cassert>
#include "Engine/Convert.h"
#include "Engine/manager/ShaderManager.h"
#include <sstream>
#include "Engine/manager/ImGuiManager.h"
#include "Model.h"

#pragma comment(lib, "dxcompiler.lib")

ID3D12Device* Particle3D::device_ = nullptr;
ID3D12GraphicsCommandList* Particle3D::commandList_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Particle3D::rootSignature_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Particle3D::particlePipelineStates_[Particle3D::BlendMode::kCountBlend] = { nullptr };
//ID3D12PipelineState* Particle3D::pipelineState_ = nullptr;
Microsoft::WRL::ComPtr<IDxcBlob> Particle3D::vs3dParticleBlob_ = nullptr;
Microsoft::WRL::ComPtr<IDxcBlob> Particle3D::ps3dParticleBlob_ = nullptr;
int Particle3D::modelNumber_ = 0;
Particle3D::BlendMode Particle3D::currentBlendMode_ = Particle3D::BlendMode::kNormal;
std::unordered_map<std::string, std::shared_ptr<Mesh>> Particle3D::meshes_;
const char* Particle3D::BlendTexts[Particle3D::BlendMode::kCountBlend] = { "Normal", "Add", "Subtract", "Multiply", "Screen" };

void Particle3D::StaticInitialize(ID3D12Device* device) {

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
	vs3dParticleBlob_ = CompileShader(L"./resources/shaders/Particle3d.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vs3dParticleBlob_ != nullptr);

	ps3dParticleBlob_ = CompileShader(L"./resources/shaders/Particle3d.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(ps3dParticleBlob_ != nullptr);

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
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

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ作成
	D3D12_ROOT_PARAMETER rootParameters[4]{};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing; //Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing); //Tableで利用する数

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	//平行光源
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; //レジスタ番号1を使う

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
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get(); //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { vs3dParticleBlob_->GetBufferPointer(),
	vs3dParticleBlob_->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { ps3dParticleBlob_->GetBufferPointer(),
	ps3dParticleBlob_->GetBufferSize() }; //PixelShader
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
	depthStencilDesc.DepthEnable = false;
	//書き込み
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//近ければ描画
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//通常
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	//グラフィックスパイプラインを実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&particlePipelineStates_[BlendMode::kNormal]));
	assert(SUCCEEDED(hr));

	//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	//グラフィックスパイプラインを実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&particlePipelineStates_[BlendMode::kAdd]));
	assert(SUCCEEDED(hr));

	//減算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	//グラフィックスパイプラインを実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&particlePipelineStates_[BlendMode::kSubtract]));
	assert(SUCCEEDED(hr));

	//乗算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	//グラフィックスパイプラインを実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&particlePipelineStates_[BlendMode::kMultiply]));
	assert(SUCCEEDED(hr));

	//スクリーン
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	//グラフィックスパイプラインを実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&particlePipelineStates_[BlendMode::kScreen]));
	assert(SUCCEEDED(hr));

	currentBlendMode_ = BlendMode::kNormal;

}

Particle3D* Particle3D::Create(const std::string& filename, uint32_t instanceCount) {

	Particle3D* particle3d = new Particle3D();

	particle3d->Initialize(filename, instanceCount);

	return particle3d;

}

void Particle3D::Initialize(const std::string& filename, uint32_t instanceCount) {

	assert(device_);

	mesh_ = std::make_unique<Mesh>();
	mesh_->Create(filename);

	//if (meshes_.find(filename) != meshes_.end()) {

	//	mesh_ = meshes_[filename].get();

	//}
	//else {

	//	//メッシュを登録
	//	meshes_[filename] = std::make_shared<Mesh>();
	//	meshes_[filename]->Create(filename);
	//	mesh_ = meshes_[filename].get();

	//}

	texture_ = mesh_->texture_;

	instanceCount_ = instanceCount;

	//トランスフォーム情報をインスタンス数に合わせてリサイズする
	positions_.resize(instanceCount_);
	rotations_.resize(instanceCount_);
	scales_.resize(instanceCount_);
	matWorlds_.resize(instanceCount_);
	velocities_.resize(instanceCount_);

	for (uint32_t i = 0; i < instanceCount_; i++) {
		positions_[i] = Vector3::Zero();
		rotations_[i] = Vector3::Zero();
		scales_[i] = Vector3::Identity();
		matWorlds_[i] = MakeIdentity4x4();
	}

	//transformMatrix
	{

		matBuff_ = CreateBufferResource(device_, sizeof(TransformationMatrix) * instanceCount_);

		matBuff_->Map(0, nullptr, reinterpret_cast<void**>(&matTransformMap_));

		//インスタンシングの数だけループ
		for (uint32_t i = 0; i < instanceCount_; i++) {
			matTransformMap_[i].WVP = MakeIdentity4x4();
			matTransformMap_[i].World = MakeIdentity4x4();
		}

		matBuff_->Unmap(0, nullptr);

	}

	//インスタンシングリソース設定
	{

		instancingResource_ = TextureManager::GetInstance()->SetInstancingResource(instanceCount_, matBuff_);

	}

}

void Particle3D::PreDraw(ID3D12GraphicsCommandList* commandList) {

	assert(commandList_ == nullptr);

	commandList_ = commandList;

}

void Particle3D::PostDraw() {

	commandList_ = nullptr;

	modelNumber_ = 0;

}

void Particle3D::Draw(Camera* camera) {

	matBillboard_ = camera->matWorld_;

	matBillboard_.m[3][0] = 0.0f;
	matBillboard_.m[3][1] = 0.0f;
	matBillboard_.m[3][2] = 0.0f;

	for (uint32_t i = 0; i < instanceCount_; i++) {

		if (isBillboard_) {
			matWorlds_[i] = MakeScaleMatrix(scales_[i]) * matBillboard_ * MakeTranslateMatrix(positions_[i]);
		}
		else {
			matWorlds_[i] = MakeAffineMatrix(scales_[i], rotations_[i], positions_[i]);
		}

		/*Matrix4x4 worldMatrix = worldTransform[i].matWorld_;*/
		Matrix4x4 worldViewProjectionMatrix = matWorlds_[i] * camera->matViewProjection_;
		matTransformMap_[i].WVP = worldViewProjectionMatrix;
		matTransformMap_[i].World = matWorlds_[i];
		matTransformMap_[i].WorldInverseTranspose = Transpose(Inverse(matWorlds_[i]));

	}

	//PSO設定
	commandList_->SetPipelineState(particlePipelineStates_[currentBlendMode_].Get());
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	//プリミティブ形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->SetGraphicsRootDescriptorTable(1, instancingResource_.srvHandleGPU);
	
	commandList_->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);

	//描画
	mesh_->SetCommandMesh(commandList_, instanceCount_);

}

void Particle3D::Finalize() {

	/*pipelineState_->Release();*/
	/*for (int i = BlendMode::kCountBlend - 1; i >= 0; i--) {
		particlePipelineStates_[i]->Release();
	}
	rootSignature_->Release();
	ps3dParticleBlob_->Release();
	vs3dParticleBlob_->Release();*/

}

void Particle3D::ImGuiUpdate() {

	////識別ナンバー設定(ImGuiで使用)
	//std::string name = "model Color";

	//name += std::to_string(modelNumber_);

	//ImGui::Begin("model Settings");
	//ImGui::ColorEdit4(name.c_str(), &constMap_->color.x);
	//ImGui::End();

	//modelNumber_++;

}

//デバッグ時に使用
void Particle3D::StaticImGuiUpdate() {

#ifdef _DEBUG

	ImGui::Begin("Static particle Settings");
	ImGui::Combo("Blend", (int*)&currentBlendMode_, BlendTexts, IM_ARRAYSIZE(BlendTexts));
	ImGui::End();

#endif // _DEBUG

}
