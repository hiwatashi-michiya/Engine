#include "LockOn.h"
#include "Engine/input/Input.h"
#include <cmath>
#include "Engine/manager/ImGuiManager.h"

LockOn::LockOn()
{
}

LockOn::~LockOn()
{
}

void LockOn::Initialize() {

	tex_ = TextureManager::GetInstance()->Load("./resources/lockon/mark.png");

	lockOnMark_.reset(Sprite::Create(tex_, { 640.0f,360.0f }));

}

void LockOn::Debug() {
#ifdef _DEBUG
	ImGui::Begin("lock on");
	ImGui::Checkbox("is auto", &isAuto_);
	ImGui::End();
#endif // _DEBUG
}

void LockOn::Update(const std::list<std::shared_ptr<Enemy>>& enemies,const Matrix4x4& matView) {
	
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_B)) {

		//自動ロックオン切り替え
		if (isAuto_) {
			isAuto_ = false;
		}
		else {
			isAuto_ = true;
		}

	}

	if (lockOnCoolTime_ > 0) {
		lockOnCoolTime_--;
	}

	//ロックオン状態のとき
	if (target_) {

		{

			if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_Y)) {
				//ロックオンを外す
				target_ = nullptr;
			}
			//範囲外判定
			else if (IsOutOfRange()) {
				//ロックオンを外す
				target_ = nullptr;
			}
			//敵が死んだかどうか判定
			else if (target_->GetIsDead()) {
				//ロックオンを外す
				target_ = nullptr;
			}

			//再度サーチ
			if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_X)) {

				Search(enemies, matView);
				
			}

		}

	}
	else if(isAuto_){

		if (lockOnCoolTime_ <= 0) {
			Search(enemies, matView);
		}

	}
	else {

		if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_Y)) {
			Search(enemies, matView);
		}

	}

	//ロックオン継続
	if (target_) {
		//敵のロックオン座標取得
		Vector3 positionWorld = target_->GetModelWorldPos();
		//ワールド座標からスクリーン座標に変換
		Vector3 positionScreen = TransformScreen(positionWorld, Model::matView_, Model::matProjection_);
		//Vector2に格納
		Vector2 positionScreenV2(positionScreen.x - 32.0f, positionScreen.y - 32.0f);
		lockOnMark_->SetPosition(positionScreenV2);
	}

}

void LockOn::Draw() {

	if (target_) {
		lockOnMark_->Draw();
	}

}

void LockOn::Search(const std::list<std::shared_ptr<Enemy>>& enemies, const Matrix4x4& matView) {

	//目標
	std::list<std::pair<float, Enemy*>> targets;

	//全ての敵に対して順にロックオン判定
	for (const auto& enemy : enemies) {

		/*Vector3 positionWorld = enemy->GetModelWorldPos();*/
		Vector3 positionWorld = enemy->GetWorldPosition();

		//ワールド->ビュー座標変換
		Vector3 positionView = CoordTransform(positionWorld, matView);

		//敵が死んでいなければ
		if (enemy->GetIsDead() == false) {

			//ターゲットと同一でない場合
			if (enemy.get() != target_) {

				//距離条件チェック
				if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {

					//カメラ前方との角度を計算
					float arcTangent = atan2(
						std::sqrtf(positionView.x * positionView.x + positionView.y * positionView.y),
						positionView.z);

					//角度条件チェック
					if (fabsf(arcTangent) <= angleRange_) {
						targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
					}

				}

			}

		}

	}

	//ロックオン対象をリセット
	target_ = nullptr;
	//空でなければ
	if (!targets.empty()) {
		//距離で昇順にソート
		targets.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
		//ソートの結果一番近い敵をロックオン対象とする
		target_ = targets.front().second;
		lockOnCoolTime_ = maxLockOnCoolTime_;
	}


}

bool LockOn::IsOutOfRange() {

	/*Vector3 positionWorld = target_->GetModelWorldPos();*/
	Vector3 positionWorld = target_->GetWorldPosition();

	//ワールド->ビュー座標変換
	Vector3 positionView = CoordTransform(positionWorld, Model::matView_);

	//距離条件チェック
	if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {

		//カメラ前方との角度を計算
		float arcTangent = atan2(
			std::sqrtf(positionView.x * positionView.x + positionView.y * positionView.y),
			positionView.z);

		//角度条件チェック
		if (fabsf(arcTangent) <= angleRange_) {
			return false;
		}

	}

	//範囲外である
	return true;

}

void LockOn::SearchRange() {


}
