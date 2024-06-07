#pragma once
#include "Transform.h"
#include <memory>
#include <string>

class GameObject
{
public:
	GameObject() : transform_(std::make_unique<Transform>()) {};
	~GameObject() = default;



protected:

	std::unique_ptr<Transform> transform_;

	std::string name_;

};


