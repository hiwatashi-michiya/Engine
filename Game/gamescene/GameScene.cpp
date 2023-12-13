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

	rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f,0.71f,0.0f }, 0.3f);
	rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f,0.0f,0.71f }, 3.141592f);

	interpolate0 = Slerp(rotation0, rotation1, 0.0f);
	interpolate1 = Slerp(rotation0, rotation1, 0.3f);
	interpolate2 = Slerp(rotation0, rotation1, 0.5f);
	interpolate3 = Slerp(rotation0, rotation1, 0.7f);
	interpolate4 = Slerp(rotation0, rotation1, 1.0f);

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("result");
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : interpolate0, slerp(q0, q1, 0.0f)",
		interpolate0.x, interpolate0.y, interpolate0.z, interpolate0.w);
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : interpolate1, slerp(q0, q1, 0.3f)",
		interpolate1.x, interpolate1.y, interpolate1.z, interpolate1.w);
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : interpolate2, slerp(q0, q1, 0.5f)",
		interpolate2.x, interpolate2.y, interpolate2.z, interpolate2.w);
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : interpolate3, slerp(q0, q1, 0.7f)",
		interpolate3.x, interpolate3.y, interpolate3.z, interpolate3.w);
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : interpolate4, slerp(q0, q1, 1.0f)",
		interpolate4.x, interpolate4.y, interpolate4.z, interpolate4.w);
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
