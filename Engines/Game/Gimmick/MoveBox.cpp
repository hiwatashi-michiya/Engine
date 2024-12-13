#include "MoveBox.h"
#include "Rand.h"
#include "Game/stage/Stage.h"
#include "Game/ColorSetter/ColorSetter.h"
#include "ColorHolder.h"

MoveBox::MoveBox()
{
	model_.reset(Model::Create("./Resources/block/block.obj"));
	circleTex_ = TextureManager::GetInstance()->Load("./Resources/block/circle.png");
	crossTex_ = TextureManager::GetInstance()->Load("./Resources/block/cross.png");
	collider_ = std::make_unique<BoxCollider>();
	lineBox_ = std::make_unique<LineBox>();

}

MoveBox::~MoveBox()
{
}

void MoveBox::Initialize() {

	name_ = "moveBox";
	collider_->SetGameObject(this);
	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;
	preTranslate_ = collider_->collider_.center;
	collider_->SetFunction([this](Collider* collider) {OnCollision(collider); });
	lineBox_->SetOBB(&collider_->collider_);

}

void MoveBox::Update() {

	//色が同じなら動かすこと可能
	if (GameColor::CheckIsActiveColor(color_, Stage::stageColor_, ColorHolder::GetHolderColor())) {
		name_ = "moveBox";
		model_->SetTexture(circleTex_);
	}
	//違うならブロックと同じ扱い
	else {
		name_ = "block";
		model_->SetTexture(crossTex_);
	}

	//座標関連更新
	preTranslate_ = collider_->collider_.center;

	collider_->collider_.center = transform_->translate_;
	collider_->collider_.size = transform_->scale_;

	transform_->UpdateMatrix();

	lineBox_->Update();

	model_->SetWorldMatrix(transform_->worldMatrix_);

	model_->SetColor(CreateColor(color_));

}

void MoveBox::Draw(Camera* camera) {

	model_->Draw(camera);
	lineBox_->Draw(camera);

}

void MoveBox::OnCollision(Collider* collider) {

	if (collider->GetGameObject()->GetName() == "player" || collider->GetGameObject()->GetName() == "copyCat") {
		isMove_ = true;
	}
	else {
		isMove_ = false;
	}

	
}
