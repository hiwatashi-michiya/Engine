#include "GameScene.h"
#include "SceneManager.h"
#include "PostEffectDrawer.h"
#include "CollisionManager.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

	skyDome_.reset(Model::Create("./Resources/skydome/temp.obj"));
	skybox_ = std::make_unique<Skybox>();
	skybox_->LoadDss("./Resources/textures/rostock_laage_airport_4k.dds");
	pauseTex_ = TextureManager::GetInstance()->Load("./Resources/UI/pause.png");
	pauseSprite_.reset(Sprite::Create(pauseTex_, { 640.0f - 256.0f,360.0f - 64.0f }));

}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();
	gv_ = GlobalVariables::GetInstance();
	mapEditor_ = MapEditor::GetInstance();
	audioManager_ = AudioManager::GetInstance();

	mapEditor_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,35.0f, -30.0f };
	camera_->rotation_.x = 0.9f;
	mapEditor_->SetCamera(camera_.get());

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->SetCamera(camera_.get());

	skyDomeTransform_ = std::make_unique<Transform>();
	skyDomeTransform_->scale_ = { 500.0f,500.0f,500.0f };
	skyDome_->material_->dLightMap_->direction = { 0.0f,0.0f,1.0f };
	skyDome_->material_->dLightMap_->intensity = 5.0f;
	skyDome_->material_->pLightMap_->radius = 650.0f;
	skyDome_->material_->pLightMap_->intensity = 0.0f;
	skyDome_->material_->pLightMap_->decay = 0.5f;

	stage_ = std::make_unique<Stage>();
	stage_->Initialize();
	stage_->LoadStage(stageNumber_);
	followCamera_->SetTarget(stage_->GetPlayer()->GetTransform());
	stage_->GetPlayer()->SetCamera(camera_.get());

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	/*skyDome_->ImGuiUpdate("skyDome");*/

	ImGui::Begin("manual");
	ImGui::Text("Move : Left Stick");
	ImGui::Text("Camera : Right Stick");
	ImGui::Text("Jump : A Button");
	ImGui::Text("Pause : Start Button");
	ImGui::Text("Reset : R key");
	ImGui::End();

	ImGui::Begin("Scene Change");
	ImGui::Text("Key 1 or 2 + L_ctrl: Change Scene\n1 : title\n2 : select");
	ImGui::End();

	if (input_->TriggerKey(DIK_1) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else if (input_->TriggerKey(DIK_2) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("SELECT");
	}
	else if (input_->TriggerKey(DIK_4) && input_->PushKey(DIK_LCONTROL)) {
		SceneManager::GetInstance()->ChangeScene("EDITOR");
	}

	ImGui::Begin("Frame Late");
	ImGui::Text("%1.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();

	PostEffectDrawer::GetInstance()->Debug();

	if (input_->TriggerKey(DIK_R)) {
		stage_->Initialize();
		stage_->LoadStage(stageNumber_);
		resetCount_ = 60;
	}

	/*mapEditor_->Edit();*/

#endif // _DEBUG

	

	if (isPause_) {

		if (input_->TriggerButton(Input::Button::START)) {
			isPause_ = false;
		}

		PostEffectDrawer::GetInstance()->SetType(kGaussianFilter);

	}
	else {

		if (input_->TriggerButton(Input::Button::START)) {
			isPause_ = true;
		}

		if (stage_->GetPlayer()->GetIsGoal()) {

			if (stageNumber_ < kMaxStage_) {
				stageNumber_++;
			}

			stage_->Initialize();
			stage_->LoadStage(stageNumber_);
			resetCount_ = 60;

		}

		if (stage_->GetPlayer()->GetIsDead()) {

			resetCount_--;

			if (resetCount_ <= 0) {
				stage_->Initialize();
				stage_->LoadStage(stageNumber_);
				resetCount_ = 60;
			}

			PostEffectDrawer::GetInstance()->SetType(kGrayscale);

		}
		else {
			PostEffectDrawer::GetInstance()->SetType(kDepthBasedOutline);
		}

		stage_->Update();

		skyDomeTransform_->UpdateMatrix();
		skyDome_->SetWorldMatrix(skyDomeTransform_->worldMatrix_);
		skybox_->SetWorldMatrix(skyDomeTransform_->worldMatrix_);

		PostEffectDrawer::GetInstance()->SetCamera(camera_.get());

		CollisionManager::GetInstance()->CheckAllCollisions();

		followCamera_->Update();

	}
	
	/*Vector3 mousePos3D = { input_->GetMousePosition().x, input_->GetMousePosition().y, 0.0f };

	mousePos3D = CoordTransform(mousePos3D, Inverse(MakeViewportMatrix(
		0, 0, 1280.0f, 720.0f, 0.0f, 1.0f)));
	mousePos3D = CoordTransform(mousePos3D, Inverse(camera_->matProjection_));
	mousePos3D = CoordTransform(mousePos3D, Inverse(camera_->matView_));

	line_->start_ = camera_->position_;
	line_->end_ = mousePos3D;*/

}

void GameScene::DrawModel() {

	/*Model::PostDraw();

	Skybox::PreDraw(dxSetter_->GetCommandList());

	skybox_->Draw(camera_.get());

	Skybox::PostDraw();*/

	skyDome_->Draw(camera_.get());

	/*Model::PreDraw(dxSetter_->GetCommandList());*/

	stage_->Draw(camera_.get());

}

void GameScene::DrawSkinningModel() {

	stage_->DrawSkinningModel(camera_.get());

}

void GameScene::DrawParticle() {

	stage_->DrawParticle(camera_.get());
}

void GameScene::DrawSprite() {

	if (isPause_) {
		pauseSprite_->Draw();
	}

}

void GameScene::DrawLine() {

	stage_->DrawLine(camera_.get());

}
