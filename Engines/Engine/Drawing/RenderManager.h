#pragma once
#include "Model.h"
#include "Skinning/SkinningModel.h"
#include "Particle3D.h"
#include "Sprite.h"
#include "LineDrawer.h"
#include <vector>

/// <summary>
/// 描画コマンドを集めて実際の描画を行うクラス
/// </summary>
class RenderManager
{
public:
	
	static RenderManager* GetInstance();
	//全てを一旦クリアする
	void Clear();
	//描画
	void Render();
	//モデルの追加
	void AddModel(Model* model) { models_.push_back(model); }
	//スキニングモデルの追加
	void AddSkinningModel(SkinningModel* skinningModel) { skinningModels_.push_back(skinningModel); }
	//パーティクルの追加
	void AddParticle3D(Particle3D* particle) { particles_.push_back(particle); }
	//スプライトの追加
	void AddSprite(Sprite* sprite) { sprites_.push_back(sprite); }
	//ラインの追加
	void AddLine(Line* line) { lines_.push_back(line); }

private:

	//描画する対象のポインタを集めたリスト
	std::vector<Model*> models_;
	std::vector<SkinningModel*> skinningModels_;
	std::vector<Particle3D*> particles_;
	std::vector<Sprite*> sprites_;
	std::vector<Line*> lines_;


private:
	//シングルトン化
	RenderManager() = default;
	~RenderManager() = default;
	RenderManager(const RenderManager&) = delete;
	const RenderManager& operator=(const RenderManager&) = delete;

};

