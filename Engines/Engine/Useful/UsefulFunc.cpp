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

