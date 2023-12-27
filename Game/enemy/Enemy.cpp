#include "Enemy.h"
#include "Engine/math/Rand.h"

Enemy::Enemy()
{
	model_.reset(Model::Create("./resources/cube/cube.obj"));
}

Enemy::~Enemy()
{
}

void Enemy::Initialize() {

	model_->position_ = { 0.0f,10.0f,50.0f };
	model_->scale_ *= 20.0f;
	collision_.center = model_->position_;
	collision_.radius = 20.0f;

	workAttack_.attackInterval = 300;
	workAttack_.attackCount = 3;
	workAttack_.attackTimer = workAttack_.attackInterval;

}

void Enemy::Update() {

	if (!isDead_) {

		/*if (--workAttack_.attackTimer <= 0) {
			Attack();
			workAttack_.attackTimer = workAttack_.attackInterval;
		}*/

		if (hp_ <= 0) {
			isDead_ = true;
		}

		collision_.center = model_->position_;

	}

}

void Enemy::Attack() {

	if (blocksPtr_) {

		for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
			std::shared_ptr<Block> block = std::make_shared<Block>();
			block->Initialize(player_->GetPosition() + Vector3{ float(rand() % 5 + 1), 3.0f, float(rand() % 5 + 1) },
				player_, {float(rand() % 5 + 1), float(rand() % 5 + 1) , float(rand() % 5 + 1) });
			blocksPtr_->push_back(block);
		}

	}

}

void Enemy::Draw(Camera* camera) {

	model_->Draw(camera);

}
