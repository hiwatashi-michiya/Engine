#include <filesystem>
#include <cassert>
#include <fstream>
#include "ModelManager.h"

ModelManager* ModelManager::GetInstance()
{
	static ModelManager instance;
	return &instance;
}
