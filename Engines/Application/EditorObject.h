#pragma once
#include "Model.h"
#include "Transform.h"
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

//マップに配置されているオブジェクトの構造体
class MapObject {
public:

	MapObject();
	~MapObject() = default;

	virtual void Initialize(const std::string& name);

	virtual void Edit();

	virtual void Draw(Camera* camera);

	std::string objName_;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Transform> transform_;

	float* matrix_;

	//オブジェクトの役割を表すタグ
	std::string tag_;

	bool isSelect_ = false;

	//削除に使用するフラグ
	bool isDelete_ = false;

	bool isOpen_ = false;

	bool preOpen_ = false;

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

class RingObject : public MapObject
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

	void Edit() override;

	void Draw(Camera* camera) override;

};