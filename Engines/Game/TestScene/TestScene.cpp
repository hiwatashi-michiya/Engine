#include "TestScene.h"
#include "FrameWork/SceneManager.h"
#include "PostEffectDrawer.h"
#include <cmath>

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

TestScene::TestScene()
{

}

TestScene::~TestScene()
{
}

void TestScene::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();
	audioManager_ = AudioManager::GetInstance();

	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();
	camera_->GetCamera()->position_ = {0.0f,2.0f, -60.0f};
	camera_->GetCamera()->rotation_.x = 0.0f;

	testATex_ = TextureManager::GetInstance()->Load("./Resources/textures/head_cake_jpg.dds");
	testBTex_ = TextureManager::GetInstance()->Load("./Resources/textures/image_drape_jpg.dds");
	testCTex_ = TextureManager::GetInstance()->Load("./Resources/textures/head_s_moun_jpg.dds");
	testDTex_ = TextureManager::GetInstance()->Load("./Resources/textures/head_cake_jpg.dds");
	testETex_ = TextureManager::GetInstance()->Load("./Resources/textures/head_trees_jpg.dds");

	testA_.reset(Sprite::Create(testATex_, {100.0f,100.0f}));
	testB_.reset(Sprite::Create(testBTex_, {300.0f,100.0f}));
	testC_.reset(Sprite::Create(testCTex_, {500.0f,100.0f}));
	testD_.reset(Model::Create("./Resources/block/block.obj"));
	testE_.reset(Model::Create("./Resources/block/block.obj"));
	testD_->SetTexture(testDTex_);
	testE_->SetTexture(testETex_);
	testDTransform_ = std::make_unique<Transform>();
	testETransform_ = std::make_unique<Transform>();
	testETransform_->translate_ = { 3.0f,3.0f,3.0f };
	testETransform_->UpdateMatrix();
	testE_->SetWorldMatrix(testETransform_->worldMatrix_);

}

void TestScene::Finalize()
{
}

void TestScene::Update() {

#ifdef _DEBUG

	Vector3 moves{};

	moves.x = input_->GetMouseMove().x;
	moves.y = input_->GetMouseMove().y;
	moves.z = float(input_->GetMouseWheel());

	ImGui::Begin("Frame Late");
	ImGui::Text("%1.2f", ImGui::GetIO().Framerate);
	ImGui::End();

#endif // _DEBUG
	
	camera_->Update();


}

void TestScene::Draw()
{

	testA_->Draw();
	testB_->Draw();
	testC_->Draw();
	testD_->Draw(camera_->GetCamera());
	testE_->Draw(camera_->GetCamera());

}
