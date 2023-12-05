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

struct Material
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

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

MaterialData LoadMaterialTemplateFile(const std::string& filename);

ModelData LoadObjFile(const std::string& filename);
