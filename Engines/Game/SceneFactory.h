#pragma once

#include "FrameWork/AbsSceneFactory.h"

/// <summary>
/// ゲームのシーン生成を担当するクラス
/// </summary>
class SceneFactory : public AbsSceneFactory
{
public:
	
	/// <summary>
	/// 次のシーン生成
	/// </summary>
	/// <param name="sceneName">シーンの名前</param>
	/// <returns></returns>
	BaseScene* CreateScene(const std::string& sceneName) override;

private:



};

