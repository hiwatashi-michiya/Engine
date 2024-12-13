#pragma once
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "TextureManager.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <map>
#include <string>
#include <vector>

//モデルに必要な構造体などを纏めたヘッダー

//頂点データ
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	float padding;
};
//マテリアルデータ
struct MaterialData
{
	Vector4 color;
	
	Vector4 secondColor;

	int32_t enableLighting;
	float shininess; //光沢度

	//dissolve
	float threshold;
	float edgeValue;
	Vector3 edgeColor;

	//ノイズ
	int32_t isActiveNoise;

	Matrix4x4 uvTransform;

};
//平行光源
struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};
//点光源
struct PointLight {
	Vector4 color; //ライトの色
	Vector3 position; //ライトの位置
	float intensity; //輝度
	float radius; //ライトの届く最大距離
	float decay; //減衰率
	float padding[2];
};
//画像データ
struct TextureData {
	std::string textureFilePath;
};
//メッシュデータ
struct MeshData {
	std::vector<VertexData> vertices;
};

//マップエディター用のデータ構造体
struct ObjectData
{
	//オブジェクトの形状を決める名前
	std::string objName;
	//位置
	Vector3 position;
	//回転
	Vector3 rotation;
	//スケール
	Vector3 scale;

};
//オイラー角トランスフォーム
struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
//クォータニオントランスフォーム
struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};
//ノード
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix; //ノードのトランスフォーム
	std::string name; //ノードの名前
	std::vector<Node> children; //子供のノード
};
//頂点の重みデータ
struct VertexWeightData {
	
	float weight;
	uint32_t vertexIndex;

};
//ジョイントの重みデータ
struct JointWeightData {

	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;

};
//スキニングモデルデータ
struct ModelData {
	std::map <std::string, JointWeightData> skinClusterData;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
	Node rootNode;
};

//TextureData LoadMaterialTemplateFile(const std::string& filename);

//MeshData LoadModelFile(const std::string& filename);
