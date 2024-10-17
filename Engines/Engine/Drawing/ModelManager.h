#pragma once
#include "Model.h"

/// <summary>
/// モデルの描画管理などを行うクラス
/// </summary>
class ModelManager
{
public:
	
	static ModelManager* GetInstance();



private:



private:

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	const ModelManager& operator=(const ModelManager&) = delete;

};

