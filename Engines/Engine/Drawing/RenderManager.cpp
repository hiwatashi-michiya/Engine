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

	DirectXSetter::GetInstance()->RenderTexturePreDraw();

	Model::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < models_.size(); i++) {

	}

	Model::PostDraw();

	SkinningModel::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < skinningModels_.size(); i++) {

	}

	SkinningModel::PostDraw();

	Particle3D::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < particles_.size(); i++) {

	}

	Particle3D::PostDraw();

	DirectXSetter::GetInstance()->PreDraw();

	PostEffectDrawer::GetInstance()->Draw();

	Sprite::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < sprites_.size(); i++) {

	}

	Sprite::PostDraw();

	Line::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	for (int32_t i = 0; i < lines_.size(); i++) {

	}

	Line::PostDraw();

}
