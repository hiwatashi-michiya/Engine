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

	Vector3 from0 = Normalize(Vector3{ 1.0f,0.7f,0.5f });
	Vector3 to0 = -from0;
	Vector3 from1 = Normalize(Vector3{ -0.6f,0.9f,0.2f });
	Vector3 to1 = Normalize(Vector3{ 0.4f,0.7f,-0.5f });

	rotateMatrix0_ = DirectionToDirection(
		Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(Vector3{ 1.0f,0.0f,0.0f }));
	rotateMatrix1_ = DirectionToDirection(from0, to0);
	rotateMatrix2_ = DirectionToDirection(from1, to1);

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("rotateMatrices");
	ImGui::Text("rotateMatrix0");
	ImGui::Text("m0 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix0_.m[0][0], rotateMatrix0_.m[0][1], rotateMatrix0_.m[0][2], rotateMatrix0_.m[0][3]);
	ImGui::Text("m1 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix0_.m[1][0], rotateMatrix0_.m[1][1], rotateMatrix0_.m[1][2], rotateMatrix0_.m[1][3]);
	ImGui::Text("m2 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix0_.m[2][0], rotateMatrix0_.m[2][1], rotateMatrix0_.m[2][2], rotateMatrix0_.m[2][3]);
	ImGui::Text("m3 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix0_.m[3][0], rotateMatrix0_.m[3][1], rotateMatrix0_.m[3][2], rotateMatrix0_.m[3][3]);
	ImGui::Text("rotateMatrix1");
	ImGui::Text("m0 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix1_.m[0][0], rotateMatrix1_.m[0][1], rotateMatrix1_.m[0][2], rotateMatrix1_.m[0][3]);
	ImGui::Text("m1 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix1_.m[1][0], rotateMatrix1_.m[1][1], rotateMatrix1_.m[1][2], rotateMatrix1_.m[1][3]);
	ImGui::Text("m2 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix1_.m[2][0], rotateMatrix1_.m[2][1], rotateMatrix1_.m[2][2], rotateMatrix1_.m[2][3]);
	ImGui::Text("m3 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix1_.m[3][0], rotateMatrix1_.m[3][1], rotateMatrix1_.m[3][2], rotateMatrix1_.m[3][3]);
	ImGui::Text("rotateMatrix2");
	ImGui::Text("m0 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix2_.m[0][0], rotateMatrix2_.m[0][1], rotateMatrix2_.m[0][2], rotateMatrix2_.m[0][3]);
	ImGui::Text("m1 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix2_.m[1][0], rotateMatrix2_.m[1][1], rotateMatrix2_.m[1][2], rotateMatrix2_.m[1][3]);
	ImGui::Text("m2 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix2_.m[2][0], rotateMatrix2_.m[2][1], rotateMatrix2_.m[2][2], rotateMatrix2_.m[2][3]);
	ImGui::Text("m3 : %1.3f %1.3f %1.3f %1.3f", rotateMatrix2_.m[3][0], rotateMatrix2_.m[3][1], rotateMatrix2_.m[3][2], rotateMatrix2_.m[3][3]);
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
