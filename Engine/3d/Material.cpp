#include "Material.h"
#include <cassert>
#include "Engine/manager/ShaderManager.h"
#include <filesystem>
#include <fstream>
#include "Engine/manager/ImGuiManager.h"

ID3D12Device* Material::device_ = nullptr;


void Material::StaticInitialize(ID3D12Device* device) {

	assert(device);

	device_ = device;

}

//mtlファイル読み込み関数
void Material::LoadMaterialTemplateFile(const std::string& filename) {

	//1.中で必要となる変数の宣言
	TextureData textureData; //構築するtextureData
	std::string line; //ファイルから読んだ1行を格納するもの

	//2.ファイルを開く
	std::ifstream file(filename); //ファイルを開く
	assert(file.is_open()); //開けなかったら止める

	//3.実際にファイルを読み、textureDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			//フルパス取得
			std::filesystem::path fullPath(filename);
			//画像ファイルのパス取得
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			textureData.textureFilePath = fullPath.parent_path().string() + "/" + textureFilename;

		}

	}

	//テクスチャ設定
	{

		//テクスチャ情報が空でない場合に設定
		if (textureData.textureFilePath != "") {
			texture_ = TextureManager::GetInstance()->Load(textureData.textureFilePath);
		}
		//テクスチャ情報が空の場合、既定の画像に設定
		else {
			texture_ = TextureManager::GetInstance()->Load("resources/sample/white.png");
		}

	}

}

Material* Material::Create(const std::string& filename) {

	//定数バッファ
	{

		constBuff_ = CreateBufferResource(device_, sizeof(MaterialData));

		//マッピングしてデータ転送
		constBuff_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));

		constMap_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		constMap_->enableLighting = true;
		constMap_->shininess = 50.0f;
		constMap_->uvTransform = MakeIdentity4x4();

		//アンマップ
		constBuff_->Unmap(0, nullptr);

	}

	//平行光源バッファ設定
	{

		dLightBuff_ = CreateBufferResource(device_, sizeof(DirectionalLight));

		dLightBuff_->Map(0, nullptr, reinterpret_cast<void**>(&dLightMap_));

		dLightMap_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		dLightMap_->direction = { 0.0f,-1.0f,0.0f };
		dLightMap_->intensity = 0.0f;

		dLightBuff_->Unmap(0, nullptr);

	}

	//点光源バッファ設定
	{

		pLightBuff_ = CreateBufferResource(device_, sizeof(PointLight));

		pLightBuff_->Map(0, nullptr, reinterpret_cast<void**>(&pLightMap_));

		pLightMap_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		pLightMap_->position = { 0.0f,0.0f,0.0f };
		pLightMap_->intensity = 1.0f;
		pLightMap_->radius = 200.0f;
		pLightMap_->decay = 1.0f;

	}

	LoadMaterialTemplateFile(filename);

	return this;

}

void Material::SetCommandMaterial(ID3D12GraphicsCommandList* commandList) {

	dLightMap_->direction = Normalize(dLightMap_->direction);

	//平行光源設定
	commandList->SetGraphicsRootConstantBufferView(3, dLightBuff_->GetGPUVirtualAddress());
	//点光源設定
	commandList->SetGraphicsRootConstantBufferView(5, pLightBuff_->GetGPUVirtualAddress());
	////SRVの設定
	/*commandList->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);*/
	commandList->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

}

void Material::SetCommandMaterialForParticle(ID3D12GraphicsCommandList* commandList) {

	//平行光源設定
	commandList->SetGraphicsRootConstantBufferView(3, dLightBuff_->GetGPUVirtualAddress());
	////SRVの設定
	/*commandList->SetGraphicsRootDescriptorTable(2, texture_->srvHandleGPU);*/
	commandList->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

}

void Material::ImGuiUpdate() {

#ifdef _DEBUG

	if (ImGui::TreeNode("Base Material")) {
		ImGui::ColorEdit4("C_color", &constMap_->color.x);
		ImGui::SliderInt("C_Lighting", &constMap_->enableLighting, 0, 1);
		ImGui::DragFloat("C_shininess", &constMap_->shininess, 0.05f, 0.0f, 100.0f);
		ImGui::SliderInt("C_is_gray", &constMap_->isGrayScale, 0, 1);
		ImGui::SliderInt("C_is_inversion", &constMap_->isInversion, 0, 1);
		ImGui::SliderInt("C_is_retro", &constMap_->isRetro, 0, 1);
		ImGui::SliderInt("C_is_aveblur", &constMap_->isAverageBlur, 0, 1);
		ImGui::SliderInt("C_is_emboss", &constMap_->isEmboss, 0, 1);
		ImGui::SliderInt("C_is_sharp", &constMap_->isSharpness, 0, 1);
		ImGui::SliderInt("C_is_outline", &constMap_->isOutline, 0, 1);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Directional Light")) {
		ImGui::ColorEdit4("D_color", &dLightMap_->color.x);
		ImGui::DragFloat3("D_direction", &dLightMap_->direction.x, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("D_intencity", &dLightMap_->intensity, 0.01f, 0.0f, 100.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Point Light")) {
		ImGui::ColorEdit4("P_color", &pLightMap_->color.x);
		ImGui::DragFloat3("P_position", &pLightMap_->position.x, 0.01f);
		ImGui::DragFloat("P_intencity", &pLightMap_->intensity, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("P_radius", &pLightMap_->radius, 0.01f);
		ImGui::DragFloat("P_decay", &pLightMap_->decay, 0.05f, 0.0f, 100.0f);
		ImGui::TreePop();
	}

#endif // _DEBUG

}
