#pragma once
#include <list>
#include <memory>
#include "Engine/3d/Model.h"
#include "Engine/Tool/GlobalVariables.h"
#include <string>

class MapEditor
{
public:
	
	MapEditor* GetInstance();

	void Edit();

	void Draw();

//関数
private:

	void Save();

	void Load(const std::string& filename);

	void AddObject();

//変数
private:

	GlobalVariables* globalVariables_ = nullptr;



private:

	MapEditor() = default;
	~MapEditor() = default;
	MapEditor(const MapEditor&) = delete;
	const MapEditor& operator=(const MapEditor&) = delete;

};
