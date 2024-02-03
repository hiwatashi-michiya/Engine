#pragma once

#include "BaseScene.h"
#include <string>

class AbsSceneFactory
{
public:
	
	virtual ~AbsSceneFactory() = default;

	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;

private:

};


