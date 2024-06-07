#include "Collider.h"
#include "CollisionManager.h"

Collider::Collider() { CollisionManager::GetInstance()->PushCollider(this); }

Collider::~Collider() { CollisionManager::GetInstance()->PopCollider(this); }
