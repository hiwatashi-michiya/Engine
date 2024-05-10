#include "MeshManager.h"

MeshManager* MeshManager::GetInstance() {
	static MeshManager instance;
	return &instance;
}

void MeshManager::CreateMesh(const std::string& fileName) {

	//メッシュを登録
	meshes_[fileName] = std::make_shared<Mesh>();
	meshes_[fileName]->Create(fileName);

}
