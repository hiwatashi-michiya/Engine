#include "LevelDataLoader.h"
#include <fstream>
#include <Windows.h>
#include "UsefulFunc.h"

LevelData::LevelData()
{
	
}

LevelData::~LevelData()
{
}

LevelDataLoader* LevelDataLoader::GetInstance() {
	static LevelDataLoader instance;
	return &instance;
}

void LevelDataLoader::Load(const std::string& fileName) {

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fileName);
	//失敗チェック
	if (file.fail()) {

		MessageBox(nullptr, L"ファイルを開くのに失敗しました。", L"LevelDataLoader - Load", MB_OK);

		assert(false);

	}

	//JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name =
		deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//インスタンス生成
	levelData_ = std::make_shared<LevelData>();

	//"objects"の全オブジェクト走査
	ScanObject(deserialized, "objects");

}

void LevelDataLoader::ScanObject(nlohmann::json objects, const std::string& name) {

	//name内の全オブジェクト走査
	for (nlohmann::json& object : objects[name]) {

		assert(object.contains("type"));

		//種別を取得
		std::string type = object["type"].get<std::string>();

		//MESHの場合
		if (type.compare("MESH") == 0) {

			//要素追加
			levelData_->objects_.emplace_back(LevelData::ObjectData{});
			//今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData_->objects_.back();

			objectData.type = type;

			if (object.contains("file_name")) {
				
				//ファイル名
				objectData.fileName = SearchResourceFile(object["file_name"], ".obj");
				//.objファイルが無かったら.gltfもサーチ
				if (objectData.fileName.empty()) {
					objectData.fileName = SearchResourceFile(object["file_name"], ".gltf");
				}
				//それでも無かったらリソースがないのでメッセージ出す
				if (objectData.fileName.empty()) {
					MessageBox(nullptr, L"リソースファイル内にシーンに必要なリソースが存在しません。", L"LevelDataLoader - ScanObject", MB_OK);
					assert(false);
				}

			}
			//ファイルネームがそもそも無い場合はCubeをデフォルトで設定
			else {
				objectData.fileName = "./Resources/cube/cube.obj";
			}

			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			//平行移動
			objectData.translation.x = (float)transform["translation"][1];
			objectData.translation.y = (float)transform["translation"][2];
			objectData.translation.z = -(float)transform["translation"][0];
			//回転角
			objectData.rotation.x = -(float)transform["rotation"][1];
			objectData.rotation.y = -(float)transform["rotation"][2];
			objectData.rotation.z = (float)transform["rotation"][0];
			//スケーリング
			objectData.scaling.x = (float)transform["scaling"][1];
			objectData.scaling.y = (float)transform["scaling"][2];
			objectData.scaling.z = (float)transform["scaling"][0];

		}
		//カメラの場合
		if (type.compare("CAMERA") == 0) {

			//要素追加
			levelData_->objects_.emplace_back(LevelData::ObjectData{});
			//今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData_->objects_.back();

			objectData.type = type;

			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			//平行移動
			objectData.translation.x = (float)transform["translation"][1];
			objectData.translation.y = (float)transform["translation"][2];
			objectData.translation.z = -(float)transform["translation"][0];
			//回転角。エンジンのカメラに合わせる
			objectData.rotation.x = -(float)transform["rotation"][0] + 1.570796326f;
			objectData.rotation.y = -(float)transform["rotation"][2] + 1.570796326f;
			objectData.rotation.z = -(float)transform["rotation"][1];
			//スケーリング
			objectData.scaling.x = (float)transform["scaling"][1];
			objectData.scaling.y = (float)transform["scaling"][2];
			objectData.scaling.z = (float)transform["scaling"][0];

		}

		//再帰処理
		if (object.contains("children")) {
			ScanObject(object, "children");
		}

	}

}

void LevelDataLoader::SetObjects() {



}
