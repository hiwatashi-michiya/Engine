#include "RenderManager.h"
#include "Core/DirectXSetter.h"
#include "PostEffectDrawer.h"

RenderManager* RenderManager::GetInstance()
{
	static RenderManager instance;
	return &instance;
}

void RenderManager::Clear()
{

	models_.clear();
	skinningModels_.clear();
	particles_.clear();
	sprites_.clear();
	lines_.clear();

}

void RenderManager::Render()
{
	//レンダーテクスチャの対象設定
	DirectXSetter::GetInstance()->RenderTexturePreDraw();
	//モデル描画
	Model::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < models_.size(); i++) {
		models_[i]->Render();
	}

	Model::PostDraw();
	//スキニングモデル描画描画
	SkinningModel::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < skinningModels_.size(); i++) {
		skinningModels_[i]->Render();
	}

	SkinningModel::PostDraw();
	//パーティクル描画
	Particle3D::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < particles_.size(); i++) {
		particles_[i]->Render();
	}

	Particle3D::PostDraw();
	//ここまでがポストエフェクトの対象
	DirectXSetter::GetInstance()->PreDraw();
	//ポストエフェクト適用
	PostEffectDrawer::GetInstance()->Draw();
	//スプライト描画
	Sprite::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < sprites_.size(); i++) {
		sprites_[i]->Render();
	}

	Sprite::PostDraw();
	//ライン描画
	Line::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < lines_.size(); i++) {
		lines_[i]->Render();
	}

	Line::PostDraw();
	//描画対象のリセット
	Clear();

}
