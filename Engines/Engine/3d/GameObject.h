#pragma once
#include "Transform.h"
#include <memory>
#include <string>

class GameObject
{
public:
	GameObject() : transform_(std::make_unique<Transform>()) {};
	~GameObject() = default;

	const std::string& GetName() { return name_; }

	Transform* GetTransform() { return transform_.get(); }

	void SetPosition(const Vector3& position) { transform_->translate_ = position; }

protected:

	std::unique_ptr<Transform> transform_;

	std::string name_;

};


