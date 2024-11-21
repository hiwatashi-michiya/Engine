#pragma once
#include "Model.h"
#include "Skinning/SkinningModel.h"
#include "Particle3D.h"
#include "Sprite.h"
#include "LineDrawer.h"
#include <vector>

class RenderManager
{
public:
	
	static RenderManager* GetInstance();

	void Clear();

	void Render();

	void AddModel(Model* model) { models_.push_back(model); }

	void AddSkinningModel(SkinningModel* skinningModel) { skinningModels_.push_back(skinningModel); }
	
	void AddParticle3D(Particle3D* particle) { particles_.push_back(particle); }
	
	void AddSprite(Sprite* sprite) { sprites_.push_back(sprite); }
	
	void AddLine(Line* line) { lines_.push_back(line); }

private:

	std::vector<Model*> models_;
	std::vector<SkinningModel*> skinningModels_;
	std::vector<Particle3D*> particles_;
	std::vector<Sprite*> sprites_;
	std::vector<Line*> lines_;


private:

	RenderManager() = default;
	~RenderManager() = default;
	RenderManager(const RenderManager&) = delete;
	const RenderManager& operator=(const RenderManager&) = delete;

};

