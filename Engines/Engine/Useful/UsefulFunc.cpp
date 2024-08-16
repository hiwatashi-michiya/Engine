#include "UsefulFunc.h"
#include <cassert>

std::string SearchResourceFile(const std::string& name, const std::string& extension) {

	//名無しだったら処理しない
	if (name.empty()) {
		return "";
	}

	std::filesystem::recursive_directory_iterator itr("./Resources");

	//リソースファイル内を全検索
	for (const auto& entry : itr) {

		if (std::filesystem::is_regular_file(entry.path()) &&
			entry.path().stem() == name &&
			entry.path().extension() == extension) {
			return entry.path().string();
		}

	}

	return "";

}

Vector4 CreateColor(int32_t num) {

	switch (num % 8)
	{
	default:
	case 0:
		return { 0.0f,0.5f,1.0f,1.0f };
		break;
	case 1:
		return { 1.0f,0.3f,0.0f,1.0f };
		break;
	case 2:
		return { 0.2f,1.0f,0.3f,1.0f };
		break;
	case 3:
		return { 0.0f,0.8f,0.9f,1.0f };
		break;
	case 4:
		return { 0.9f,0.1f,0.9f,1.0f };
		break;
	case 5:
		return { 0.9f,0.9f,0.1f,1.0f };
		break;
	case 6:
		return { 0.0f,0.1f,0.9f,1.0f };
		break;
	case 7:
		return { 0.9f,0.1f,0.1f,1.0f };
		break;
	}


}
