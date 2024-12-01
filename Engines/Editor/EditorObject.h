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

	virtual void Initialize(const std::string& name);

	virtual void Update();

	virtual void Edit();

	virtual void RecordMove();

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

class PlayerObject : public MapObject
{
public:

	void Initialize(const std::string& name) override;

	void Edit() override;

};

class BlockObject : public MapObject
{
public:

	void Initialize(const std::string& name) override;

	void Edit() override;

};

class MoveBoxObject : public MapObject
{
public:

	void Initialize(const std::string& name) override;

	void Edit() override;

};

class PaintObject : public MapObject
{
public:

	void Initialize(const std::string& name) override;

	void Edit() override;

};

class GoalObject : public MapObject
{
public:

	void Initialize(const std::string& name) override;

	void Edit() override;

};

class WarpObject : public MapObject
{
public:

	std::unique_ptr<Model> modelB_;

	std::unique_ptr<Transform> transformB_;

	void Initialize(const std::string& name) override;

	void Update() override;

	void Edit() override;

	void RecordMove() override;

	void Draw(Camera* camera) override;

	bool isMoveA_ = true;

	std::unique_ptr<OBB> obbB_;
	std::unique_ptr<LineBox> lineBoxB_;

};

class GhostBoxObject : public MapObject
{
public:

	void Initialize(const std::string& name) override;

	void Edit() override;

	//回転方向のフラグ
	bool isRotateRight_ = true;

};

class SwitchObject : public MapObject
{
public:

	void Initialize(const std::string& name) override;

	void Edit() override;

};

class CopyCatObject : public MapObject
{
public:

	void Initialize(const std::string& name) override;

	void Edit() override;

};

class EnemyObject : public MapObject
{
public:

	void Initialize(const std::string& name) override;

	void Edit() override;

};

class HolderObject : public MapObject
{
public:

	std::unique_ptr<Model> modelB_;

	void Initialize(const std::string& name) override;

	void Edit() override;

	void Draw(Camera* camera) override;

};
