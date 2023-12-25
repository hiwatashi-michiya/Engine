#include "block.h"
#include "Engine/math/Rand.h"

Block::Block()
{
}

Block::~Block()
{
}

void Block::Initialize(const Vector3& pos, Player* player, const Vector3& blockSize) {

	blockSize_ = blockSize;

	sizeX_ = uint32_t(fabsf(blockSize.x));
	sizeY_ = uint32_t(fabsf(blockSize.y));
	sizeZ_ = uint32_t(fabsf(blockSize.z));

	particleCount_ = sizeX_ * sizeY_ * sizeZ_;

	model_.reset(Model::Create("./resources/cube/cube.obj"));

	model_->position_ = pos;

	model_->scale_ = blockSize_;

	player_ = player;

}

void Block::SetBullet(std::list<std::shared_ptr<Bullet>>& bullets) {


	for (uint32_t z = 0; z < sizeZ_; z++) {
		for (uint32_t y = 0; y < sizeY_; y++) {
			for (uint32_t x = 0; x < sizeX_; x++) {

				Vector3 bulletPos = Vector3{ (x - sizeX_ / 2.0f) * 2.0f + 1.0f, (y - sizeY_ / 2.0f) * 2.0f + 1.0f,
					(z - sizeZ_ / 2.0f) * 2.0f + 1.0f } + model_->position_;

				std::shared_ptr<Bullet> newBullet = std::make_shared<Bullet>();
				newBullet->Initialize(bulletPos);
				bullets.push_back(newBullet);

			}
		}
	}

}

void Block::Update() {

	if (player_->GetIsBreak() && Length(GetPosition() - player_->GetPosition()) < 20.0f) {
		isDead_ = true;
	}

}

void Block::Draw(Camera* camera) {

	model_->Draw(camera);

}
