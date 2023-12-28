#include "Enemy.h"
#include "Engine/math/Rand.h"

Enemy::Enemy()
{
	model_.reset(Model::Create("./resources/cube/cube.obj"));

	for (uint32_t i = 0; i < 10; i++) {
		attackModels_[i].reset(Model::Create("./resources/cube/cube.obj"));
	}

	attackLangeTex_ = TextureManager::GetInstance()->Load("./resources/enemy/attack.png");
	blockTex_ = TextureManager::GetInstance()->Load("./resources/cube/stage.png");

}

Enemy::~Enemy()
{
}

void Enemy::Initialize() {

	model_->position_ = { 0.0f,10.0f,50.0f };
	model_->scale_ = { 20.0f,20.0f,20.0f };
	collision_.max = model_->position_ + model_->scale_;
	collision_.min = model_->position_ - model_->scale_;

	hp_ = kMaxHp_;
	isDead_ = false;

	workAttack_.attackInterval = 300;
	workAttack_.attackCount = 3;
	workAttack_.attackTimer = workAttack_.attackInterval;
	workAttack_.isStartAttack = false;
	workAttack_.startAttackInterval = 90;
	workAttack_.startAttackTimer = 0;

	workShot_.shotInterval = 180;
	workShot_.shotCount = 10;
	workShot_.shotTimer = workShot_.shotInterval;

	bullets_.clear();

}

void Enemy::Update() {

	bullets_.remove_if([](auto& bullet) {

		if (bullet->GetIsDead()) {
			return true;
		}

		return false;

		});

	if (!isDead_) {

		if (workAttack_.isStartAttack) {
			Attack();
		}
		else {

			//タイマー0で攻撃時の変数初期化、攻撃開始
			if (--workAttack_.attackTimer <= 0) {
				
				workAttack_.attackCount = rand() % 3 + 3;

				for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
					attackSizes_[i] = { float(rand() % 5 + 1), float(rand() % 4 + 2) , float(rand() % 5 + 1) };
					attackPositions_[i] = Vector3{ float(rand() % 60 - 30), attackSizes_[i].y, float(rand() % 60 - 30)};
					attackModels_[i]->scale_ = attackSizes_[i];
					attackModels_[i]->position_ = player_->GetPosition() + attackPositions_[i] - Vector3{ 0.0f, attackPositions_[i].y * 2.0f + 0.1f, 0.0f };
					attackModels_[i]->SetTexture(attackLangeTex_);
				}

				workAttack_.isStartAttack = true;
				workAttack_.attackTimer = workAttack_.attackInterval;
				workAttack_.startAttackTimer = workAttack_.startAttackInterval;

			}

		}

		if (--workShot_.shotTimer <= 0) {
			AddBullet();
			workShot_.shotTimer = workShot_.shotInterval;
		}

		if (hp_ <= 0) {
			isDead_ = true;
		}

		

	}

	for (auto& bullet : bullets_) {
		bullet->Update();

		if (IsCollision(bullet->GetCollision(), player_->GetCollision())) {
			player_->Damage(1);
			bullet->SetIsDead(true);
		}

	}

	for (auto& bullet : bullets_) {

		if (workShot_.shotTimer % 10 == 0 && !bullet->GetIsShot()) {
			bullet->Shot(player_->GetPosition());
			break;
		}

	}

}

void Enemy::Attack() {

	//カウントが30を切ったら攻撃開始
	if (--workAttack_.startAttackTimer < 30) {

		for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
			
			attackModels_[i]->position_.y += attackSizes_[i].y * 2.0f / 30.0f;

		}

		if (workAttack_.startAttackTimer <= 0) {
			
			if (blocksPtr_ && workAttack_.isStartAttack) {

				for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
					std::shared_ptr<Block> block = std::make_shared<Block>();
					block->Initialize(attackModels_[i]->position_,
						player_, attackSizes_[i]);
					blocksPtr_->push_back(block);
				}

			}

			workAttack_.isStartAttack = false;

		}

	}
	else {

	}

}

void Enemy::AddBullet() {

	for (uint32_t i = 0; i < workShot_.shotCount; i++) {

		std::shared_ptr<EnemyBullet> newBullet = std::make_shared<EnemyBullet>();

		if (i < 5) {
			newBullet->Initialize(model_->position_ + Vector3{(5.0f + i) * 5.0f, 0.0f, 0.0f});
		}
		else {
			newBullet->Initialize(model_->position_ + Vector3{i * -5.0f, 0.0f, 0.0f });
		}

		bullets_.push_back(newBullet);

	}

}

void Enemy::Draw(Camera* camera) {

	if (!isDead_) {

		model_->Draw(camera);

	}

	if (workAttack_.isStartAttack) {

		for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
			attackModels_[i]->Draw(camera);
		}

	}

	for (auto& bullet : bullets_) {
		bullet->Draw(camera);
	}

}
