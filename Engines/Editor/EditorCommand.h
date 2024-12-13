#pragma once
#include "Vector3.h"
#include <vector>
#include <memory>
#include "Transform.h"

/// <summary>
/// 何のコマンドか判断するためのタイプ
/// </summary>
enum CommandType {

	kMove, //移動
	kAdd, //追加
	kRemove, //削除

	kMaxCount,

};

/// <summary>
/// コマンドの基底クラス
/// </summary>
class ICommand
{
public:
	~ICommand() = default;
	//実行
	virtual void Execute() = 0;
	//手戻り
	virtual void Undo() = 0;
	//タイプ取得
	const CommandType& GetType() const { return type_; }

protected:
	//種類
	CommandType type_;

};

/// <summary>
/// 移動コマンド
/// </summary>
class MoveCommand : public ICommand
{
public:
	MoveCommand(Transform& target, const Transform& oldValue, const Transform& newValue) :
		target_(target), oldValue_(oldValue), newValue_(newValue) {
		type_ = kMove;
	}
	//値をセット
	void Set(Transform& target, const Transform& oldValue, const Transform& newValue) {
		target_ = target;
		oldValue_ = oldValue;
		newValue_ = newValue;
	}

	void Execute() override {
		target_ = newValue_;
	}

	void Undo() override {
		target_ = oldValue_;
	}

private:

	Transform oldValue_{};
	Transform newValue_{};
	Transform& target_;

};

/// <summary>
/// 追加コマンド
/// </summary>
/// <typeparam name="T"></typeparam>
template <class T>
class AddCommand : public ICommand
{
public:
	AddCommand(std::vector<T>& container, T object) :
		container_(container), object_(object) {
		type_ = kAdd;
	}
	//値セット
	void Set(std::vector<T>& container, T object) {
		container_ = container;
		object_ = object;
	}

	void Execute() override {
		container_.push_back(object_);
	}

	void Undo() override {
		container_.pop_back();
	}

private:

	std::vector<T>& container_;
	T object_;

};

/// <summary>
/// 削除コマンド
/// </summary>
/// <typeparam name="T"></typeparam>
template <class T>
class RemoveCommand : public ICommand
{
public:
	RemoveCommand(std::vector<T>& container, T object, int32_t index) :
		container_(container), object_(object), index_(index) {
		type_ = kRemove;
	}

	//値セット
	void Set(std::vector<T>& container, T object) {
		container_ = container;
		object_ = object;
	}

	void Execute() override {

		auto it = std::find(container_.begin(), container_.end(), object_);

		if (it != container_.end()) {
			index_ = int32_t(std::distance(container_.begin(), it));
			container_.erase(it);
		}

	}

	void Undo() override {
		container_.insert(container_.begin() + index_, object_);
	}

private:

	std::vector<T>& container_;
	T object_;
	int32_t index_;

};
