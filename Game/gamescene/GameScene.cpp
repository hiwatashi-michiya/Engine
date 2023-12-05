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

	q1 = { 2.0f,3.0f,4.0f,1.0f };
	q2 = { 1.0f,3.0f,5.0f,2.0f };

	identity.IdentityQuaternion();
	conj = ConjuGate(q1);
	inv = Inverse(q1);
	normal = Normalize(q1);
	mul1 = Multiply(q1, q2);
	mul2 = Multiply(q2, q1);

	norm = Norm(q1);
}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("rotateMatrices");
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : identity", identity.x, identity.y, identity.z, identity.w);
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : conjugate", conj.x, conj.y, conj.z, conj.w);
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : inverse", inv.x, inv.y, inv.z, inv.w);
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : normalize", normal.x, normal.y, normal.z, normal.w);
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : multiply(q1,q2)", mul1.x, mul1.y, mul1.z, mul1.w);
	ImGui::Text("%1.2f, %1.2f, %1.2f, %1.2f : multiply(q2,q1)", mul2.x, mul2.y, mul2.z, mul2.w);
	ImGui::Text("%1.2f : norm", norm);
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
