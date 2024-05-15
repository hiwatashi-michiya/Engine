#pragma once
#include "3d/Mesh.h"
#include <string>
#include <unordered_map>

/// <summary>
/// メッシュの一元管理マネージャー
/// </summary>
class MeshManager
{
public:
	
	static MeshManager* GetInstance();

	/// <summary>
	/// メッシュの作成
	/// </summary>
	/// <param name="fileName">作成に使用するファイル</param>
	void CreateMesh(const std::string& fileName);

	/// <summary>
	/// メッシュを取得する
	/// </summary>
	/// <param name="fileName">取得するファイル名</param>
	/// <returns></returns>
	Mesh* GetMesh(const std::string& fileName) { return meshes_[fileName].get(); }

	/// <summary>
	/// メッシュを既に作成しているか
	/// </summary>
	/// <param name="fileName">確認するファイルの名前</param>
	/// <returns></returns>
	bool IsExistMesh(const std::string& fileName) { return meshes_.find(fileName) != meshes_.end(); }

private:

	std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes_;

private:

	MeshManager() = default;
	~MeshManager() = default;
	MeshManager(const MeshManager&) = delete;
	const MeshManager& operator=(const MeshManager&) = delete;

};
