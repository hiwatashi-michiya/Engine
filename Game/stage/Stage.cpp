#include "Stage.h"
#include "Engine/manager/TextureManager.h"
#include "Engine/input/Input.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Initialize() {

	model_.reset(Model::Create("./resources/cube/cube.obj"));
	model_->scale_.x = 100.0f;
	model_->scale_.z = 100.0f;
	model_->position_.y = -2.0f;

	tex_ = TextureManager::GetInstance()->Load("./resources/cube/stage.png");
	model_->SetTexture(tex_);

}

void Stage::Update() {

}

void Stage::Draw() {

	model_->Draw();

}
