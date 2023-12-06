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

	rotation = MakeRotateAxisAngleQuaternion(Normalize(Vector3{ 1.0f,0.4f,-0.2f }), 0.45f);
	pointY = { 2.1f,-0.9f,1.3f };
	rotateMatrix = MakeRotateMatrix(rotation);
	rotateByQuaternion = RotateVector(pointY, rotation);
	rotateByMatrix = TransformCoord(pointY, rotateMatrix);

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("result");
	ImGui::Text("%1.2f,%1.2f,%1.2f,%1.2f : rotation", rotation.x, rotation.y, rotation.z, rotation.w);
	ImGui::Text("rotateMatrix\n%1.3f,%1.3f,%1.3f,%1.3f\n%1.3f,%1.3f,%1.3f,%1.3f\n%1.3f,%1.3f,%1.3f,%1.3f\n%1.3f,%1.3f,%1.3f,%1.3f",
		rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3],
		rotateMatrix.m[1][0], rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3],
		rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3],
		rotateMatrix.m[3][0], rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]);
	ImGui::Text("%1.2f,%1.2f,%1.2f : rotateByQuaternion", rotateByQuaternion.x, rotateByQuaternion.y, rotateByQuaternion.z);
	ImGui::Text("%1.2f,%1.2f,%1.2f : rotateByMatrix", rotateByMatrix.x, rotateByMatrix.y, rotateByMatrix.z);
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
