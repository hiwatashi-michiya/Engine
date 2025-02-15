#include "RenderManager.h"
#include "Core/DirectXSetter.h"
#include "PostEffectDrawer.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
#include "ParticleManager.h"

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
	DirectXSetter::GetInstance()->RenderTexturePreDraw(1);
	//モデル描画
	ModelManager::GetInstance()->PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	ModelManager::GetInstance()->Render();

	ModelManager::GetInstance()->PostDraw();
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
	PostEffectDrawer::GetInstance()->Draw(1);

	////レンダーテクスチャの対象設定
	//DirectXSetter::GetInstance()->RenderTexturePreDraw(0);
	////ポストエフェクト適用
	//PostEffectDrawer::GetInstance()->SetType(PostEffectType::kGrayscale);
	//PostEffectDrawer::GetInstance()->Draw(0);
	////レンダーテクスチャの対象設定
	//DirectXSetter::GetInstance()->RenderTexturePreDraw(1);
	////ポストエフェクト適用
	//PostEffectDrawer::GetInstance()->SetType(PostEffectType::kVignette);
	//PostEffectDrawer::GetInstance()->Draw(1);
	////レンダーテクスチャの対象設定
	//DirectXSetter::GetInstance()->RenderTexturePreDraw(0);
	////ポストエフェクト適用
	//PostEffectDrawer::GetInstance()->SetType(PostEffectType::kRadialBlur);
	//PostEffectDrawer::GetInstance()->Draw(0);
	////レンダーテクスチャの対象設定
	//DirectXSetter::GetInstance()->RenderTexturePreDraw(1);
	////ポストエフェクト適用
	//PostEffectDrawer::GetInstance()->SetType(PostEffectType::kHSVFilter);
	//PostEffectDrawer::GetInstance()->Draw(1);

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

#ifdef _DEBUG

	ImGuiManager::GetInstance()->Render();

#endif // _DEBUG

	//描画後処理。バリアを張る
	DirectXSetter::GetInstance()->PostDraw();

}
