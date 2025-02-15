#include "SceneChangeManager.h"
#include "SceneManager.h"
#include "Core/DirectXSetter.h"
#include "Easing.h"
#include "Vector2.h"
#include <algorithm>
#include "ParticleManager.h"

SceneChangeManager* SceneChangeManager::GetInstance() {
	static SceneChangeManager instance;
	return &instance;
}

void SceneChangeManager::Initialize() {

	texture_ = TextureManager::GetInstance()->Load("./Resources/white.png");
	circleTexture_ = TextureManager::GetInstance()->Load("./Resources/particle/particle3.png");
	sprite_.reset(Sprite::Create(texture_, {}, { 0.0f,0.0f,0.0f,1.0f }));
	sprite_->size_ = { 1280.0f,720.0f };

	for (int32_t y = 0; y < kCircleNumY_; y++) {

		for (int32_t x = 0; x < kCircleNumX_; x++) {

			circleSprites_[y][x].reset(Sprite::Create(circleTexture_, {}));
			circleSprites_[y][x]->size_ = {0.0f,0.0f};
			circleSprites_[y][x]->anchorPoint_ = {0.5f,0.5f};
			//色変更
			if (y % 3 == 0) {
				circleSprites_[y][x]->color_ = { 1.0f,0.0f,0.0f,1.0f };
			}
			else if (y % 3 == 1) {
				circleSprites_[y][x]->color_ = { 0.0f,1.0f,0.0f,1.0f };
			}
			else {
				circleSprites_[y][x]->color_ = { 0.0f,0.0f,1.0f,1.0f };
			}

			circleSprites_[y][x]->position_ = { x * 160.0f + 80.0f, y * 120.0f + 60.0f };

		}

	}

}

void SceneChangeManager::Update() {
	//シーン切り替え開始中
	if (isSceneChangeEnter_) {

		sceneChangeValue_ += sceneChangeSpeed_;

		for (int32_t y = 0; y < kCircleNumY_; y++) {

			for (int32_t x = 0; x < kCircleNumX_; x++) {

				float easingVal = (sceneChangeValue_ * 2.0f - (y * 0.08f + x * 0.06f));
				easingVal = std::clamp(easingVal, 0.0f, 1.0f);

				circleSprites_[y][x]->size_ = Lerp(Vector2{ 0.0f,0.0f }, Vector2{ 350.0f,350.0f },
					Easing::InQuad(easingVal));

			}

		}

		//値が最大値になったらシーン切り替え、終了処理に移行
		if (sceneChangeValue_ >= maxSceneChangeValue_) {
			sceneChangeValue_ = maxSceneChangeValue_;
			SceneManager::GetInstance()->ChangeScene(nextScene_);
			isSceneChangeExit_ = true;
			isSceneChangeEnter_ = false;
		}

	}
	//シーン切り替え終了中
	else if (isSceneChangeExit_) {

		sceneChangeValue_ -= sceneChangeSpeed_;

		for (int32_t y = 0; y < kCircleNumY_; y++) {

			for (int32_t x = 0; x < kCircleNumX_; x++) {

				float easingVal = sceneChangeValue_ * 2.0f - ((kCircleNumY_ - y) * 0.08f + (kCircleNumX_ - x) * 0.06f);
				easingVal = std::clamp(easingVal, 0.0f, 1.0f);

				circleSprites_[y][x]->size_ = Lerp(Vector2{ 0.0f,0.0f }, Vector2{ 350.0f,350.0f },
					Easing::InQuad(easingVal));

			}

		}

		//値が0になったら切り替え処理終了
		if (sceneChangeValue_ <= 0.0f) {
			sceneChangeValue_ = 0.0f;
			isSceneChangeExit_ = false;
			isSceneChangeEnter_ = false;

			for (int32_t y = 0; y < kCircleNumY_; y++) {

				for (int32_t x = 0; x < kCircleNumX_; x++) {

					circleSprites_[y][x]->size_ = { 0.0f,0.0f };

				}

			}

		}

	}

	sprite_->color_.w = sceneChangeValue_;

}

void SceneChangeManager::Draw() {

	/*sprite_->Draw();*/

	for (int32_t y = 0; y < kCircleNumY_; y++) {

		for (int32_t x = 0; x < kCircleNumX_; x++) {

			circleSprites_[y][x]->Draw();

		}

	}

}

void SceneChangeManager::SceneChangeStart() {

	sceneChangeValue_ = 0.0f;
	isSceneChangeEnter_ = true;

}
