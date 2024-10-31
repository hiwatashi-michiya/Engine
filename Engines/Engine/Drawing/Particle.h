#pragma once
#include "Particle3D.h"
#include <memory>
#include <string>
#include "Camera.h"
#include "Vector4.h"

class Particle
{
public:
	Particle();
	~Particle();

	void Initialize();

	void Update();

	void Draw(Camera* camera);

	//パーティクルのランダム速度最小設定
	void SetMinSpeed(const Vector3& speed) { minSpeed_ = speed; }

	//パーティクルのランダム速度最大設定
	void SetMaxSpeed(const Vector3& speed) { maxSpeed_ = speed; }

	//両方を同時設定
	void SetMinMaxSpeed(const Vector3& minSpeed, const Vector3& maxSpeed) {
		minSpeed_ = minSpeed;
		maxSpeed_ = maxSpeed;
	}

	//パーティクルの発生時の最小スケール
	void SetMinScale(float scale) { minScale_ = scale; }

	//パーティクルの発生時の最大スケール
	void SetMaxScale(float scale) { maxScale_ = scale; }

	//両方を同時設定
	void SetMinMaxScale(float minScale, float maxScale) {
		minScale_ = minScale;
		maxScale_ = maxScale;
	}

	//パーティクルの発生範囲最小
	void SetMinSpawnPoint(const Vector3& point) { minSpawnPoint_ = point; }

	//パーティクルの発生範囲最大
	void SetMaxSpawnPoint(const Vector3& point) { maxSpawnPoint_ = point; }

	//両方を同時設定
	void SetMinMaxSpawnPoint(const Vector3& minPoint, const Vector3& maxPoint) {
		minSpawnPoint_ = minPoint;
		maxSpawnPoint_ = maxPoint;
	}

	//パーティクルの速度変化量(一定)
	void SetChangeSpeed(const Vector3& speed) { changeSpeed_ = speed; }

	//パーティクルのサイズ変化量(一定)
	void SetChangeScale(float scale) { changeScale_ = scale; }

	//パーティクルの最短生存時間
	void SetMinLifeTime(int32_t lifeTime) { minLifeTime_ = lifeTime; }

	//パーティクルの最長生存時間
	void SetMaxLifeTime(int32_t lifeTime) { maxLifeTime_ = lifeTime; }

	//両方を同時設定
	void SetMinMaxLifeTime(int32_t minTime, int32_t maxTime) {
		minLifeTime_ = minTime;
		maxLifeTime_ = maxTime;
	}

	//パーティクル全体の発生時間
	void SetParticleLifeTime(int32_t lifeTime) { particleLifeTime_ = lifeTime; }

	//色変更
	void SetColor(const Vector4& color) { color_ = color; }

	//パーティクルの最大発生数
	void SetInstanceCount(uint32_t count) {

		if (count > kMaxParticle_) {
			count = kMaxParticle_;
		}

		instanceCount_ = count;
		particle_->SetInstanceCount(instanceCount_);

	}

	void SetIsLoop(bool flag) { isLoop_ = flag; }

	bool IsEnd();

private:

	std::shared_ptr<Particle3D> particle_;

	static const uint32_t kMaxParticle_ = 128;

	//速度の最低値
	Vector3 minSpeed_{};

	Vector3 maxSpeed_{};

	float minScale_ = 0.1f;

	float maxScale_ = 1.0f;

	Vector3 minSpawnPoint_{};

	Vector3 maxSpawnPoint_{};
	
	Vector3 changeSpeed_{};

	float changeScale_ = 0.0f;

	Vector3 changeRotate_{};

	int32_t minLifeTime_ = 60;

	int32_t maxLifeTime_ = 60;

	uint32_t instanceCount_ = 32;

	int32_t particleLifeTime_ = 120;

	bool isLoop_ = false;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

};
