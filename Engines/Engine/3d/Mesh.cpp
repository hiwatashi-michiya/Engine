#include "Mesh.h"
#include <cassert>
#include "Buffer/BufferResource.h"
#include <filesystem>
#include <fstream>
#include "Externals/imgui/imgui.h"

ID3D12Device* Mesh::device_ = nullptr;

void Mesh::StaticInitialize(ID3D12Device* device) {

	assert(device);

	device_ = device;

}

//mtlファイル読み込み関数
void Mesh::LoadMaterialTemplateFile(const std::string& filename) {

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

void Mesh::LoadObjFile(const std::string& filename) {

	//1.必要となる変数の宣言
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals; //法線
	std::vector<Vector2> texcoords; //テクスチャ座標
	std::string line; //ファイルから読んだ1行を格納するもの

	//2.ファイルを開く
	std::ifstream file(filename); //ファイルを開く
	assert(file.is_open()); //開けなかったら止める

	//3.実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; //先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			//座標系の変換のため、zを反転
			position.z *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			//座標系の変換のため、zを反転
			normal.z *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); //区切りでインデックスを読んでいく

					if (index == "") {
						//情報が無い場合、1を代入
						elementIndices[element] = 1;
					}
					else {
						elementIndices[element] = std::stoi(index);
					}

				}

				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];

				Vector2 texcoord;
				if (!texcoords.empty()) {
					texcoord = texcoords[elementIndices[1] - 1];
				}
				else {
					texcoord = { 0.0f,0.0f };
				}
				Vector3 normal = normals[elementIndices[2] - 1];
				/*VertexData vertex = { position, texcoord, normal };
				modelData.vertices.push_back(vertex);*/
				triangle[faceVertex] = { position, texcoord, normal };

			}
			//頂点を逆順で登録することで、回り順を逆にする
			meshData_.vertices.push_back(triangle[2]);
			meshData_.vertices.push_back(triangle[1]);
			meshData_.vertices.push_back(triangle[0]);

		}
		else if (identifier == "mtllib") {
			//ファイルのフルパスを取得
			std::filesystem::path fullPath(filename);
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//マテリアルを生成
			material_ = std::make_unique<Material>();
			material_->Create(fullPath.parent_path().string() + "/" + materialFilename);
			LoadMaterialTemplateFile(fullPath.parent_path().string() + "/" + materialFilename);

		}

	}

}

Mesh* Mesh::Create(const std::string& filename) {

	assert(device_);

	LoadObjFile(filename);

	//頂点バッファ
	{

		vertBuff_ = CreateBufferResource(device_, sizeof(VertexData) * meshData_.vertices.size());

		//頂点バッファビュー設定
		vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
		vbView_.SizeInBytes = UINT(sizeof(VertexData) * meshData_.vertices.size());
		vbView_.StrideInBytes = sizeof(VertexData);

		//マッピングしてデータ転送
		vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap_));

		std::memcpy(vertMap_, meshData_.vertices.data(), sizeof(VertexData) * meshData_.vertices.size());

		//アンマップ
		vertBuff_->Unmap(0, nullptr);

	}

	return this;

}

void Mesh::SetCommandMesh(ID3D12GraphicsCommandList* commandList) {

	material_->SetCommandMaterial(commandList);

	//頂点バッファビューの設定
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	//描画
	commandList->DrawInstanced(UINT(meshData_.vertices.size()), 1, 0, 0);


}

void Mesh::SetCommandMesh(ID3D12GraphicsCommandList* commandList, uint32_t instanceCount) {

	material_->SetCommandMaterialForParticle(commandList);

	//頂点バッファビューの設定
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	//描画
	commandList->DrawInstanced(UINT(meshData_.vertices.size()), instanceCount, 0, 0);


}

void Mesh::ImGuiUpdate() {

	material_->ImGuiUpdate();

}
