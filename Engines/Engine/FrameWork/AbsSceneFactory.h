#pragma once

#include "BaseScene.h"
#include <string>

/// <summary>
/// エンジン側のシーンファクトリー
/// </summary>
class AbsSceneFactory
{
public:
	
	virtual ~AbsSceneFactory() = default;
	//シーン生成
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;

private:

};


