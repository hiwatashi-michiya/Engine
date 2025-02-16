#pragma once
#include "Particle3D.h"
#include <memory>
#include <string>
#include "Camera.h"
#include "Vector4.h"

//パーティクルエディタの前方宣言
class ParticleEditor;

/// <summary>
/// パーティクルを汎用的にしたクラス
/// </summary>
class Particle
{
	//パーティクルエディタも編集可能にする
	friend class ParticleEditor;
public:
	Particle();
	~Particle();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);

	//パラメータをデフォルトに設定
	void SetDefault();

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
	//ベースとなる座標を設定
	void SetSpawnPoint(const Vector3& point) { spawnPoint_ = point; }
	//追従するターゲット設定
	void SetFollowPoint(Vector3* followPoint) { followPoint_ = followPoint; }
	//追従するターゲット設定
	void SetTargetPoint(Vector3* target) { targetPoint_ = target; }
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
	void SetColor(const Vector4& color) { 
		minStartColor_ = color;
		maxStartColor_ = color;
		endColor_ = color;
	}

	//色変更
	void SetMinStartColor(const Vector4& color) { minStartColor_ = color; }
	void SetMaxStartColor(const Vector4& color) { maxStartColor_ = color; }

	//色変更
	void SetEndColor(const Vector4& color) { endColor_ = color; }

	//パーティクルの最大発生数
	void SetInstanceCount(int32_t count) {

		if (count > kMaxParticle_) {
			count = kMaxParticle_;
		}

		instanceCount_ = count;
		particle_->SetInstanceCount(instanceCount_);

	}
	//ループフラグセット
	void SetIsLoop(bool flag) { isLoop_ = flag; }
	//終了したかどうか
	bool IsEnd();
	//名前取得
	const std::string& GetName() { return name_; }
	//テクスチャセット
	void SetTexture(const std::string& texture) { particle_->SetTexture(texture); }
	//データ読み込み
	void Load(const std::string& filePath);

private:

	std::shared_ptr<Particle3D> particle_;
	//最大発生数
	static const uint32_t kMaxParticle_ = 128;
	//名前
	std::string name_;
	//出現時の色範囲
	Vector4 minStartColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 maxStartColor_ = { 1.0f,1.0f,1.0f,1.0f };
	//消滅時の色
	Vector4 endColor_ = { 1.0f,1.0f,1.0f,1.0f };
	//速度最低値
	Vector3 minSpeed_{};
	//速度最大値
	Vector3 maxSpeed_{};
	//スポーン地点
	Vector3 spawnPoint_{};
	//スポーン範囲最小
	Vector3 minSpawnPoint_{};
	//スポーン範囲最大
	Vector3 maxSpawnPoint_{};
	//スピードの変化量(一定)
	Vector3 changeSpeed_{};
	//回転変化量
	Vector3 changeRotate_{};
	//スケール変化量(一定)
	float changeScale_ = 0.0f;
	//起点座標
	Vector3* followPoint_ = nullptr;
	//ターゲット座標
	Vector3* targetPoint_ = nullptr;
	//最小スケール
	float minScale_ = 0.1f;
	//最大スケール
	float maxScale_ = 1.0f;
	//最小生存時間
	int32_t minLifeTime_ = 60;
	//最大生存時間
	int32_t maxLifeTime_ = 60;
	//インスタンス数
	int32_t instanceCount_ = 32;
	//パーティクル全体の生存時間
	int32_t particleLifeTime_ = 120;
	//パーティクル全体の最大生存時間
	int32_t maxParticleLifeTime_ = 120;
	//アクティブ状態かどうか
	bool isActive_ = true;
	//ループするかどうか
	bool isLoop_ = false;

};
