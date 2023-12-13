#include "block.h"

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
	particle_.reset(Particle3D::Create("./resources/cube/cube.obj", particleCount_));
	particle_->isBillboard_ = false;

	model_->position_ = pos;

	model_->scale_ = blockSize_;

	{

		uint32_t i = 0;

		for (uint32_t z = 0; z < sizeZ_; z++) {
			for (uint32_t y = 0; y < sizeY_; y++) {
				for (uint32_t x = 0; x < sizeX_; x++) {
					particle_->positions_[i] = Vector3{ (x - sizeX_ / 2.0f) * 2.0f, (y - sizeY_ / 2.0f) * 2.0f,(z - sizeZ_ / 2.0f) * 2.0f } + model_->position_;
					particle_->scales_[i] = { 0.7f,0.7f,0.7f };
					/*particle_->rotations_[i] = { i * 0.3f, i * 0.1f, i * -0.2f };*/
					i++;
				}
			}
		}

	}

	player_ = player;

}

void Block::Update() {

	if (player_->GetIsBreak() && Length(GetPosition() - player_->GetPosition()) < 20.0f) {
		isBreak_ = true;
	}

	if (isBreak_ && Length(GetPosition() - player_->GetPosition()) < 6.0f) {

		isDead_ = true;
	}

}

void Block::Draw() {

	if (isBreak_) {
		particle_->Draw();
	}
	else {
		model_->Draw();
	}

}
