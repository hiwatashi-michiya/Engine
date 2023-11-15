#include "GameScene.h"

#ifdef _DEBUG

#include "Engine/manager/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{
	model_.reset(Model::Create("./resources/plane/plane.obj"));
}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audioManager_ = AudioManager::GetInstance();
	Model::worldTransformCamera_.translation_.z = -10.0f;

	Vector3 axis = { Normalize({1.0f,1.0f,1.0f}) };
	float angle = 0.44f;
	rotateMatrix_ = MakeRotateAxisAngle(axis, angle);

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("rotateMatrix");
	ImGui::Text("m0 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix_.m[0][0], rotateMatrix_.m[0][1], rotateMatrix_.m[0][2], rotateMatrix_.m[0][3]);
	ImGui::Text("m1 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix_.m[1][0], rotateMatrix_.m[1][1], rotateMatrix_.m[1][2], rotateMatrix_.m[1][3]);
	ImGui::Text("m2 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix_.m[2][0], rotateMatrix_.m[2][1], rotateMatrix_.m[2][2], rotateMatrix_.m[2][3]);
	ImGui::Text("m3 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix_.m[3][0], rotateMatrix_.m[3][1], rotateMatrix_.m[3][2], rotateMatrix_.m[3][3]);
	ImGui::End();

#endif // _DEBUG

}

void GameScene::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//3Dモデル描画
	{

		Model::PreDraw(commandList);

		/*model_->Draw(worldTransform_);*/

		Model::PostDraw();

	}
	
	//パーティクル描画
	{

		Particle3D::PreDraw(commandList);

		

		Particle3D::PostDraw();

	}

	//2Dスプライト描画
	{

		Sprite::PreDraw(commandList);



		Sprite::PostDraw();

	}


}
