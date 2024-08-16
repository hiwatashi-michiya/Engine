#include "GameScene.h"
#include "SceneManager.h"
#include "PostEffectDrawer.h"
#include "CollisionManager.h"
#include "UsefulFunc.h"

#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

	skyDome_.reset(Model::Create("./Resources/skydome/temp.obj"));
	skybox_ = std::make_unique<Skybox>();
	skybox_->LoadDds("./Resources/skydome/test.dds");
	pauseTex_ = TextureManager::GetInstance()->Load("./Resources/UI/pause.png");
	pauseSprite_.reset(Sprite::Create(pauseTex_, { 640.0f - 256.0f,360.0f - 64.0f }));
	menuTex_ = TextureManager::GetInstance()->Load("./Resources/UI/start.png");
	menuSprite_.reset(Sprite::Create(menuTex_, { 1240.0f - 32.0f,60.0f - 32.0f }));
	leftTex_ = TextureManager::GetInstance()->Load("./Resources/UI/L_Stick.png");
	leftSprite_.reset(Sprite::Create(leftTex_, { 140.0f - 128.0f,660.0f - 32.0f }));
	rightTex_ = TextureManager::GetInstance()->Load("./Resources/UI/R_Stick.png");
	rightSprite_.reset(Sprite::Create(rightTex_, { 1140.0f - 128.0f,660.0f - 32.0f }));
	toGameTex_ = TextureManager::GetInstance()->Load("./Resources/UI/toGame.png");
	toGameSprite_.reset(Sprite::Create(toGameTex_, { 340.0f - 256.0f,560.0f - 32.0f }));
	toSelectTex_ = TextureManager::GetInstance()->Load("./Resources/UI/toSelect.png");
	toSelectSprite_.reset(Sprite::Create(toSelectTex_, { 940.0f - 256.0f,560.0f - 32.0f }));

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
	skyDome_->material_->dLightMap_->direction = { 0.0f,-0.8f,0.6f };
	skyDome_->material_->dLightMap_->intensity = 1.0f;
	skyDome_->material_->pLightMap_->radius = 650.0f;
	skyDome_->material_->pLightMap_->intensity = 0.15f;
	skyDome_->material_->pLightMap_->decay = 0.5f;

	stage_ = std::make_unique<Stage>();
	stage_->Initialize();
	stage_->LoadStage(stageNumber_);
	followCamera_->SetTarget(stage_->GetPlayer()->GetTransform());
	stage_->GetPlayer()->SetCamera(camera_.get());

	PostEffectDrawer::GetInstance()->SetCamera(camera_.get());
	PostEffectDrawer::GetInstance()->SetType(kDepthBasedOutline);

	stage_->Update();

	skyDomeTransform_->UpdateMatrix();
	skyDome_->SetWorldMatrix(skyDomeTransform_->worldMatrix_);
	skybox_->SetWorldMatrix(skyDomeTransform_->worldMatrix_);
	followCamera_->Update();


}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();

	skyDome_->ImGuiUpdate("skyDome");

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

	if (isPause_ && !SceneChangeManager::GetInstance()->IsSceneChange()) {


		if (input_->TriggerKey(DIK_Q)) {
			isPause_ = false;
		}

		if (input_->TriggerKey(DIK_SPACE) && !SceneChangeManager::GetInstance()->IsSceneChange()) {
			SceneChangeManager::GetInstance()->SetNextScene("SELECT");
			SceneChangeManager::GetInstance()->SceneChangeStart();
		}

	}
	else if (!SceneChangeManager::GetInstance()->IsSceneChange()) {

		if (input_->TriggerKey(DIK_P)) {
			isPause_ = true;
		}

	}


#endif // _DEBUG

	

	if (isPause_ && !SceneChangeManager::GetInstance()->IsSceneChange()) {

		PostEffectDrawer::GetInstance()->SetType(kGaussianFilter);

		if (input_->TriggerButton(Input::Button::START) || input_->TriggerButton(Input::Button::B)) {
			isPause_ = false;
		}

		if (input_->TriggerButton(Input::Button::A) && !SceneChangeManager::GetInstance()->IsSceneChange()) {
			SceneChangeManager::GetInstance()->SetNextScene("SELECT");
			SceneChangeManager::GetInstance()->SceneChangeStart();
		}

	}
	else if(!SceneChangeManager::GetInstance()->IsSceneChange()) {

		if (input_->TriggerButton(Input::Button::START)) {
			isPause_ = true;
		}

		if (stage_->GetPlayer()->GetIsGoal()) {

			if (stageNumber_ < kMaxStage_ && stageNumber_ > 0) {
				stageNumber_++;
				SceneChangeManager::GetInstance()->SetNextScene("GAMEPLAY");
				SceneChangeManager::GetInstance()->SceneChangeStart();
			}
			else {
				SceneChangeManager::GetInstance()->SetNextScene("SELECT");
				SceneChangeManager::GetInstance()->SceneChangeStart();
			}

			/*stage_->Initialize();
			stage_->LoadStage(stageNumber_);
			resetCount_ = 60;*/

		}

		if (stage_->GetPlayer()->GetIsDead()) {

			resetCount_--;

			if (resetCount_ <= 0) {
				SceneChangeManager::GetInstance()->SetNextScene("GAMEPLAY");
				SceneChangeManager::GetInstance()->SceneChangeStart();
			}

			PostEffectDrawer::GetInstance()->SetType(kGrayscale);

		}
		else {
			PostEffectDrawer::GetInstance()->SetType(kDepthBasedOutline);
		}

		stage_->Update();

		skyDome_->SetColor(CreateColor(stage_->stageColor_));
		skyDomeTransform_->UpdateMatrix();
		skyDome_->SetWorldMatrix(skyDomeTransform_->worldMatrix_);
		skybox_->SetWorldMatrix(skyDomeTransform_->worldMatrix_);

		PostEffectDrawer::GetInstance()->SetCamera(camera_.get());

		CollisionManager::GetInstance()->CheckAllCollisions();

		if (!stage_->GetPlayer()->GetIsDead()) {
			followCamera_->Update();
		}

	}

}

void GameScene::DrawModel() {


	/*Model::PostDraw();

	Skybox::PreDraw(DirectXSetter::GetInstance()->GetCommandList());

	skybox_->Draw(camera_.get());

	Skybox::PostDraw();

	Model::PreDraw(DirectXSetter::GetInstance()->GetCommandList());*/

	skyDome_->Draw(camera_.get());

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
		toGameSprite_->Draw();
		toSelectSprite_->Draw();
	}
	else {

		menuSprite_->Draw();
		leftSprite_->Draw();
		rightSprite_->Draw();

	}

}

void GameScene::DrawLine() {

#ifdef _DEBUG

#endif // _DEBUG

	stage_->DrawLine(camera_.get());

}
