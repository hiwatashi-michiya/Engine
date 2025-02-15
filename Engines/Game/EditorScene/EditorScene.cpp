#include "EditorScene.h"
#include "FrameWork/SceneManager.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

EditorScene::EditorScene()
{

}

EditorScene::~EditorScene()
{
}

void EditorScene::Initialize() {

	editor_ = UniqueEditor::GetInstance();
	editor_->Initialize();
	particleEditor_ = ParticleEditor::GetInstance();
	particleEditor_->Initialize();

	particleManager_ = ParticleManager::GetInstance();

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	debugCamera_->SetPosition(kStartPosition_);
	debugCamera_->SetRotate(kStartRotate_);

	editor_->SetCamera(debugCamera_->GetCamera());

	tex_ = TextureManager::GetInstance()->Load("./Resources/UI/num.png");
	test_.reset(Sprite::Create(tex_, {}));

	currentCamera_ = debugCamera_->GetCamera();

	PostEffectDrawer::GetInstance()->SetType(kDepthBasedOutline);

}

void EditorScene::Finalize()
{

	editor_->Finalize();
	particleEditor_->Finalize();

}

void EditorScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &debugCamera_->GetCamera()->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &debugCamera_->GetCamera()->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &debugCamera_->GetCamera()->position_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("Frame Late");
	ImGui::Text("%1.2f", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Scene Change");
	ImGui::Text("Key 2 or 3 + L_ctrl: Change Scene\n2 : select\n3 : game");
	ImGui::End();

	if (input_->TriggerKey(DIK_1) and input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else if (input_->TriggerKey(DIK_2) and input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("SELECT");
	}
	else if (input_->TriggerKey(DIK_3) and input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	//パーティクルエディタが開いていなけれ編集可能
	if (not particleEditor_->GetIsOpenFile()) {
		editor_->Edit();
	}
	//ステージエディタが開いていなければ編集可能
	if (not editor_->GetIsOpenFile()) {
		particleEditor_->Update();
	}
	//エディタ関連の操作をしていない時のみデバッグカメラ移動
	if (not editor_->GetIsMove() and not editor_->GetPreIsMove() and not ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) and
		not ImGui::IsAnyItemHovered() and not ImGui::IsAnyItemActive()) {
		debugCamera_->Update();
	}

#endif // _DEBUG

	particleManager_->Update();

}

void EditorScene::Draw()
{

	particleManager_->Draw(currentCamera_);

	editor_->Draw(currentCamera_);

	particleEditor_->Draw(currentCamera_);

}

