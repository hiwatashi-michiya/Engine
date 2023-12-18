#pragma once
#include "Engine/math/Matrix4x4.h"
#include "Engine/math/Vector4.h"
#include "Engine/math/Vector3.h"
#include "Engine/math/Vector2.h"
#include"Engine/manager/TextureManager.h"
#include <string>
#include <vector>

//モデルに必要な構造体などを纏めたヘッダー

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct MaterialData
{
	Vector4 color;
	
	int32_t enableLighting;
	float padding[3];

	Matrix4x4 uvTransform;

	float shininess; //光沢度
	int32_t isGrayScale = 0; //グレースケール化のint型フラグ
	int32_t isInversion = 0; //色反転フラグ
	int32_t isRetro = 0; //レトロ化フラグ

	int32_t isAverageBlur = 0; //平均化ブラーフラグ
	int32_t isEmboss = 0; //エンボス加工
	int32_t isSharpness = 0; //シャープネス
	int32_t isOutline = 0; //輪郭線抽出

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

//TextureData LoadMaterialTemplateFile(const std::string& filename);

//MeshData LoadObjFile(const std::string& filename);
