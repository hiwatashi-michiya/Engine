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

}

void Mesh::LoadModelFile(const std::string& filename) {

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	//メッシュが無いのは未対応
	assert(scene->HasMeshes());

	//メッシュの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {

		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals()); //法線が無いメッシュは非対応
		assert(mesh->HasTextureCoords(0)); //TexCoordがないメッシュは非対応

		modelData_.vertices.resize(mesh->mNumVertices); //最初に頂点数分のメモリ確保

		//頂点の解析
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {

			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			//右手系->左手系への変換
			modelData_.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			modelData_.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			modelData_.vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };

		}

		//Indexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {

			aiFace& face = mesh->mFaces[faceIndex];
			//三角形の面のみ
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData_.indices.push_back(vertexIndex);
			}

		}

		//SkinCluster
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {

			//meshに関連付けられたJointから情報を取得する
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData_.skinClusterData[jointName];

			//InverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;

			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix({ scale.x, scale.y, scale.z },
				{ rotate.x, -rotate.y, -rotate.z, rotate.w }, { -translate.x, translate.y, translate.z });
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			//Weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
			}

		}

	}

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {

		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			//ファイルのフルパスを取得
			std::filesystem::path fullPath(filename);
			
			textureFilePath_ = fullPath.parent_path().string() + "/" + textureFilePath.C_Str();

		}

	}

	//sceneのrootNodeを読んでシーン全体の階層構造を作り上げる
	modelData_.rootNode = ReadNode(scene->mRootNode);

}

Node Mesh::ReadNode(aiNode* node) {

	Node result;
	aiVector3D scale, translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);
	result.transform.scale = { scale.x, scale.y, scale.z }; //Scaleはそのまま
	result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w }; //x軸を反転、さらに回転方向が逆なので軸を反転させる
	result.transform.translate = { -translate.x, translate.y, translate.z }; //x軸を反転
	result.localMatrix = MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);

	result.name = node->mName.C_Str(); //node名を格納
	result.children.resize(node->mNumChildren); //子供の数だけ確保

	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {

		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);

	}

	return result;

}

Mesh* Mesh::Create(const std::string& filename) {

	assert(device_);

	LoadModelFile(filename);

	//頂点バッファ
	{

		vertBuff_ = CreateBufferResource(device_, sizeof(VertexData) * modelData_.vertices.size());

		vertBuff_->SetName(L"vertBuff");

		//頂点バッファビュー設定
		vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
		vbView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
		vbView_.StrideInBytes = sizeof(VertexData);

		//マッピングしてデータ転送
		vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap_));

		std::memcpy(vertMap_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

		//アンマップ
		vertBuff_->Unmap(0, nullptr);

	}

	//インデックスバッファ
	{

		indexBuff_ = CreateBufferResource(device_, sizeof(uint32_t) * modelData_.indices.size());

		ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
		ibView_.SizeInBytes = sizeof(uint32_t) * UINT(modelData_.indices.size());
		ibView_.Format = DXGI_FORMAT_R32_UINT;

		indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap_));
		std::memcpy(indexMap_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());

		indexBuff_->Unmap(0, nullptr);

	}

	return this;

}

void Mesh::SetCommandMesh(ID3D12GraphicsCommandList* commandList) {

	//頂点バッファビューの設定
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	//インデックスバッファビューの設定
	commandList->IASetIndexBuffer(&ibView_);

	//描画
	/*commandList->DrawInstanced(UINT(meshData_.vertices.size()), 1, 0, 0);*/
	commandList->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);

}

void Mesh::SetCommandMesh(ID3D12GraphicsCommandList* commandList, uint32_t instanceCount) {

	//頂点バッファビューの設定
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	//インデックスバッファビューの設定
	commandList->IASetIndexBuffer(&ibView_);

	//描画
	/*commandList->DrawInstanced(UINT(meshData_.vertices.size()), instanceCount, 0, 0);*/
	commandList->DrawIndexedInstanced(UINT(modelData_.indices.size()), instanceCount, 0, 0, 0);

}

void Mesh::SetCommandMeshForSkinning(ID3D12GraphicsCommandList* commandList) {

	//インデックスバッファビューの設定
	commandList->IASetIndexBuffer(&ibView_);

	//描画
	/*commandList->DrawInstanced(UINT(meshData_.vertices.size()), 1, 0, 0);*/
	commandList->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);

}

void Mesh::ImGuiUpdate() {

}
