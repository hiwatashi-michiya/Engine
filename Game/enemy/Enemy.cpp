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

	hpTex_ = TextureManager::GetInstance()->Load("./resources/e_life.png");

	hpSprite_.reset(Sprite::Create(hpTex_, { 490.0f,20.0f }));
	hpSprite_->size_.x = hpWidth_ * hp_;
	hpSprite_->size_.y = 64.0f;

}

Enemy::~Enemy()
{
}

void Enemy::Initialize() {

	audioManager_ = AudioManager::GetInstance();

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

	hpSprite_->size_.x = hpWidth_ * hp_;
	hpSprite_->size_.y = 64.0f;

	bullets_.clear();

	groundAttackSE_ = audioManager_->SoundLoadWave("./resources/proto_sound/boss_groundAttack.wav");
	shotSE_ = audioManager_->SoundLoadWave("./resources/proto_sound/boss_shot.wav");
	deathSE_ = audioManager_->SoundLoadWave("./resources/proto_sound/disolve.wav");

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
					attackCollisions_[i].max = attackModels_[i]->position_ + attackSizes_[i];
					attackCollisions_[i].min = attackModels_[i]->position_ - attackSizes_[i];
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
			audioManager_->Play(deathSE_, 0.2f);
		}

		

	}

	for (auto& bullet : bullets_) {
		bullet->Update();

		if (IsCollision(bullet->GetCollision(), player_->GetCollision()) && !player_->GetIsInvincible()) {
			player_->Damage(1);
			bullet->SetIsDead(true);
		}

	}

	for (auto& bullet : bullets_) {

		if (workShot_.shotTimer % 10 == 0 && workShot_.shotTimer <= 120 && !bullet->GetIsShot()) {
			bullet->Shot(player_->GetPosition());
			audioManager_->Play(shotSE_, 0.2f);
			break;
		}

	}

	hpSprite_->size_.x = hpWidth_ * hp_;

}

void Enemy::Attack() {

	//カウントが30を切ったら攻撃開始
	if (--workAttack_.startAttackTimer < 30) {

		for (uint32_t i = 0; i < workAttack_.attackCount; i++) {
			
			attackModels_[i]->position_.y += attackSizes_[i].y * 2.0f / 30.0f;

			if (IsCollision(attackCollisions_[i], player_->GetCollision())) {
				player_->Damage(3);
			}

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

		if (workAttack_.startAttackTimer == 30) {
			audioManager_->Play(groundAttackSE_, 0.2f);
		}

	}

}

void Enemy::AddBullet() {

	for (uint32_t i = 0; i < workShot_.shotCount; i++) {

		std::shared_ptr<EnemyBullet> newBullet = std::make_shared<EnemyBullet>();

		if (i < 5) {
			newBullet->Initialize(model_->position_ + Vector3{(5.0f + i) * 5.0f, float(rand() % 10 - 5), float(rand() % 10 - 5) });
		}
		else {
			newBullet->Initialize(model_->position_ + Vector3{i * -5.0f, float(rand() % 10 - 5), float(rand() % 10 - 5) });
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

void Enemy::DrawTexture() {

	hpSprite_->Draw();

}
