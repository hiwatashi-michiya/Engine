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

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	float padding;
};

struct MaterialData
{
	Vector4 color;
	
	int32_t enableLighting;
	float shininess; //光沢度

	//dissolve
	float threshold;
	float edgeValue;
	Vector3 edgeColor;

	//ノイズ
	int32_t isActiveNoise;

	Matrix4x4 uvTransform;
	
	//int32_t isGrayScale = 0; //グレースケール化のint型フラグ
	//int32_t isInversion = 0; //色反転フラグ
	//int32_t isRetro = 0; //レトロ化フラグ

	//int32_t isAverageBlur = 0; //平均化ブラーフラグ
	//int32_t isEmboss = 0; //エンボス加工
	//int32_t isSharpness = 0; //シャープネス
	//int32_t isOutline = 0; //輪郭線抽出

};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct PointLight {
	Vector4 color; //ライトの色
	Vector3 position; //ライトの位置
	float intensity; //輝度
	float radius; //ライトの届く最大距離
	float decay; //減衰率
	float padding[2];
};

struct TextureData {
	std::string textureFilePath;
};

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

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix; //ノードのトランスフォーム
	std::string name; //ノードの名前
	std::vector<Node> children; //子供のノード
};

struct VertexWeightData {
	
	float weight;
	uint32_t vertexIndex;

};

struct JointWeightData {

	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;

};

struct ModelData {
	std::map <std::string, JointWeightData> skinClusterData;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
	Node rootNode;
};

//TextureData LoadMaterialTemplateFile(const std::string& filename);

//MeshData LoadModelFile(const std::string& filename);
