#pragma once

#include "FrameWork/AbsSceneFactory.h"

class SceneFactory : public AbsSceneFactory
{
public:
	
	BaseScene* CreateScene(const std::string& sceneName) override;

private:



};

