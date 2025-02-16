#pragma once
#include "Model.h"
#include "Transform.h"
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Vector3.h"
#include "LineDrawer.h"
#include "EditorCommand.h"
#include "Game/ColorSetter/ColorSetter.h"
#include <stack>

//マップに配置されているオブジェクトの構造体
class MapObject {
public:

	MapObject();
	~MapObject() = default;
	//初期化
	virtual void Initialize(const std::string& name);
	//更新
	virtual void Update();
	//編集
	virtual void Edit();
	//変更記録
	virtual void RecordMove();
	//描画コマンド積む
	virtual void Draw(Camera* camera);

	std::string objName_;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Transform> transform_;

	std::unique_ptr<Transform> oldTransform_;

	float* matrix_;

	//オブジェクトの役割を表すタグ
	std::string tag_;

	bool isSelect_ = false;

	//削除に使用するフラグ
	bool isDelete_ = false;

	bool isOpen_ = false;

	bool preOpen_ = false;

	GameColor::Color color_ = GameColor::kWhite;

	std::unique_ptr<OBB> obb_;
	std::unique_ptr<LineBox> lineBox_;

	bool isUsing_ = false;
	bool preIsUsing_ = false;

protected:

	static std::stack<std::shared_ptr<ICommand>>& undoCommands_;
	static std::stack<std::shared_ptr<ICommand>>& redoCommands_;

};

/// <summary>
/// プレイヤー
/// </summary>
class PlayerObject : public MapObject
{
public:

	PlayerObject(){}
	~PlayerObject(){}

	void Initialize(const std::string& name) override;

	void Edit() override;

};

/// <summary>
/// ブロック
/// </summary>
class BlockObject : public MapObject
{
public:

	BlockObject() {}
	~BlockObject() {}

	void Initialize(const std::string& name) override;

	void Edit() override;

};

/// <summary>
/// ダイブ可能な箱
/// </summary>
class GhostBoxObject : public MapObject
{
public:

	GhostBoxObject() {}
	~GhostBoxObject() {}

	void Initialize(const std::string& name) override;

	void Edit() override;

	//回転方向のフラグ
	bool isRotateRight_ = true;

};

/// <summary>
/// 色切り替え
/// </summary>
class SwitchObject : public MapObject
{
public:

	SwitchObject() {}
	~SwitchObject() {}

	void Initialize(const std::string& name) override;

	void Edit() override;

};

/// <summary>
/// 入ったら画像を出すエリア
/// </summary>
class SpriteFlagObject : public MapObject
{
public:

	SpriteFlagObject() {}
	~SpriteFlagObject() {}

	void Initialize(const std::string& name) override;

	void Edit() override;

};
