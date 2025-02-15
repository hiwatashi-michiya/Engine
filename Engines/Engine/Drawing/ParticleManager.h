#pragma once
#include "Particle3D.h"
#include "Vector3.h"
#include <memory>
#include <list>
#include "Particle.h"

/// <summary>
/// パーティクルの管理を行うクラス
/// </summary>
class ParticleManager
{
public:
	//インスタンス取得
	static ParticleManager* GetInstance();
	//初期化
	void Initialize();
	//終了処理
	void Finalize();
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);
	//全パーティクルのリセット
	void Reset();
	//未使用のパーティクル取得
	bool GetUnUsedParticle(Particle*& particle);
	//パーティクル最大数
	static const int32_t kMaxParticle_ = 256;

private:
	//管理しているパーティクルのリスト
	std::array<std::unique_ptr<Particle>, kMaxParticle_> particles_;
	//どのパーティクルが現在使われているか
	std::array<bool, kMaxParticle_> isUsed_;

private:
	//シングルトン化
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

};


