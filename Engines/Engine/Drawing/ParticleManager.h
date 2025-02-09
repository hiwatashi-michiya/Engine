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
	//更新
	void Update();
	//描画
	void Draw(Camera* camera);
	//パーティクル追加
	void AddParticle(std::unique_ptr<Particle>& particle);

private:
	//管理しているパーティクルのリスト
	std::list<std::unique_ptr<Particle>> particles_;

private:
	//シングルトン化
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

};


