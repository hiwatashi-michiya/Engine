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

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();

	cameraX_ = std::make_unique<Camera>();
	cameraX_->Initialize();
	cameraX_->position_ = { 60.0f,0.0f, 0.0f };
	cameraX_->rotation_.y = -1.57f;

	cameraY_ = std::make_unique<Camera>();
	cameraY_->Initialize();
	cameraY_->position_ = { 0.0f, 60.0f, 0.0f };
	cameraY_->rotation_.x = 1.57f;

	cameraZ_ = std::make_unique<Camera>();
	cameraZ_->Initialize();
	cameraZ_->position_ = { 0.0f,0.0f, 60.0f };
	cameraZ_->rotation_.y = -3.14f;

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	debugCamera_->SetPosition({ 0.0f,65.0f, -60.0f });
	debugCamera_->SetRotate({ 0.9f,0.0f,0.0f });

	editor_->SetCamera(debugCamera_->GetCamera());

	tex_ = TextureManager::GetInstance()->Load("./Resources/UI/num.png");
	test_.reset(Sprite::Create(tex_, {}));

	currentCamera_ = debugCamera_->GetCamera();

}

void EditorScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &debugCamera_->GetCamera()->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &debugCamera_->GetCamera()->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &debugCamera_->GetCamera()->position_.x, 0.1f);
	ImGui::End();

	test_->ImGuiUpdate("test");

	ImGui::Begin("Scene Change");
	ImGui::Text("Key 2 or 3 + L_ctrl: Change Scene\n2 : select\n3 : game");
	ImGui::End();

	if (input_->TriggerKey(DIK_1) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else if (input_->TriggerKey(DIK_2) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("SELECT");
	}
	else if (input_->TriggerKey(DIK_3) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	editor_->Edit();

	if (!editor_->GetIsMove() && !editor_->GetPreIsMove() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
		debugCamera_->Update();
	}

	if (input_->TriggerKey(DIK_Q) and input_->PushKey(DIK_LCONTROL)) {

		cameraIndex_++;

		if (cameraIndex_ > 3) {
			cameraIndex_ = 0;
		}

		switch (cameraIndex_)
		{
		default:
		case 0:
			currentCamera_ = debugCamera_->GetCamera();
			break;
		case 1:
			currentCamera_ = cameraX_.get();
			break;
		case 2:
			currentCamera_ = cameraY_.get();
			break;
		case 3:
			currentCamera_ = cameraZ_.get();
			break;
		}

	}

#endif // _DEBUG

	cameraX_->position_ = debugCamera_->GetCamera()->position_ + Vector3{60.0f, -65.0f,60.0f};
	cameraY_->position_ = debugCamera_->GetCamera()->position_ + Vector3{ 0.0f, -5.0f,60.0f };
	cameraZ_->position_ = debugCamera_->GetCamera()->position_ + Vector3{ 0.0f, -65.0f,120.0f };

	cameraX_->Update();
	cameraY_->Update();
	cameraZ_->Update();

}

void EditorScene::DrawModel() {

	editor_->Draw(currentCamera_);

}

void EditorScene::DrawSkinningModel() {

}

void EditorScene::DrawParticle() {



}

void EditorScene::DrawSprite() {

	/*test_->Draw();*/

}

void EditorScene::DrawLine() {

	editor_->DrawLine(currentCamera_);

}
