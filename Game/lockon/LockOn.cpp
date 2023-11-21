#include "LockOn.h"
#include "Engine/input/Input.h"

LockOn::LockOn()
{
}

LockOn::~LockOn()
{
}

void LockOn::Initialize() {

	tex_ = TextureManager::GetInstance()->Load("./resources/lockon/mark.png");

	lockOnMark_.reset(Sprite::Create(tex_, { 640.0f,360.0f }));

	isLockOn_ = false;

}

void LockOn::Update(const std::list<std::shared_ptr<Enemy>>& enemies,const Matrix4x4& matView) {

	if (isLockOn_) {

	}
	else {

		if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_Y)) {
			Search(enemies, matView);
		}

	}

	if (isLockOn_) {

	}

}

void LockOn::Draw() {

	lockOnMark_->Draw();

}

void LockOn::Search(const std::list<std::shared_ptr<Enemy>>& enemies, const Matrix4x4& matView) {

	//目標
	std::list<std::pair<float, const Enemy*>> targets;

	//全ての敵に対して順にロックオン判定
	for (const auto& enemy : enemies) {

		Vector3 positionWorld = enemy->GetModelWorldPos();

		//ワールド->ビュー変換行列
		Vector3 positionView = CoordTransform(positionWorld, matView);

		//距離条件チェック
		if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {

			//カメラ前方との角度を計算


		}

	}

}
