#pragma once
#include "Vector3.h"
#include <vector>
#include <memory>
#include "Transform.h"

class ICommand
{
public:
	~ICommand() = default;

	virtual void Execute() = 0;

	virtual void Undo() = 0;

};

class MoveCommand : public ICommand
{
public:
	MoveCommand(Transform& target, const Transform& oldValue, const Transform& newValue) :
		target_(target), oldValue_(oldValue), newValue_(newValue){}

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

template <class T>
class AddCommand : public ICommand
{
public:
	AddCommand(std::vector<T>& container, T object) :
	container_(container), object_(object){}

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

template <class T>
class RemoveCommand : public ICommand
{
public:
	RemoveCommand(std::vector<T>& container, T object) :
		container_(container), object_(object), index_(0) {}

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
