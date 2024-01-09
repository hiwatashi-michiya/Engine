#include "GameScene.h"

#ifdef _DEBUG

#include "Engine/manager/ImGuiManager.h"

#endif // _DEBUG

GameScene::GameScene()
{

	blackTex_ = TextureManager::GetInstance()->Load("./resources/UI/black.png");
	fade_.reset(Sprite::Create(blackTex_, { 0.0f,0.0f }));
	fade_->size_ = { 1280.0f, 720.0f };
	fade_->color_.w = 0.0f;
	skydome_.reset(Model::Create("./resources/skydome/skydome.obj"));
	skydome_->scale_ *= 500.0f;

	stage1Tex_ = TextureManager::GetInstance()->Load("./resources/UI/stage1.png");
	stage2Tex_ = TextureManager::GetInstance()->Load("./resources/UI/stage2.png");
	stage3Tex_ = TextureManager::GetInstance()->Load("./resources/UI/stage3.png");
	selectTileTex_ = TextureManager::GetInstance()->Load("./resources/UI/selecttile.png");

	push_A_Tex_ = TextureManager::GetInstance()->Load("./resources/UI/push_a.png");
	moveTex_ = TextureManager::GetInstance()->Load("./resources/UI/move.png");
	selectTex_ = TextureManager::GetInstance()->Load("./resources/UI/select.png");
	stageSelectTex_ = TextureManager::GetInstance()->Load("./resources/UI/stageselect.png");
	stageClearTex_ = TextureManager::GetInstance()->Load("./resources/UI/stageclear.png");
	titleTex_ = TextureManager::GetInstance()->Load("./resources/UI/title.png");

	stage1Sprite_.reset(Sprite::Create(stage1Tex_, { 300.0f,300.0f }));
	stage2Sprite_.reset(Sprite::Create(stage2Tex_, { 576.0f,300.0f }));
	stage3Sprite_.reset(Sprite::Create(stage3Tex_, { 852.0f,300.0f }));
	selectTileSprite_.reset(Sprite::Create(selectTileTex_, { -8.0f + (276.0f * stageNumber_),268.0f}));
	selectTileSprite_->size_ = { 192.0f,192.0f };

	push_A_Sprite_.reset(Sprite::Create(push_A_Tex_, { 640.0f - 256.0f, 500.0f }));
	push_A_Sprite_->size_ = { 512.0f,128.0f };
	moveSprite_.reset(Sprite::Create(moveTex_, { 1030.0f,600.0f }));
	selectSprite_.reset(Sprite::Create(selectTex_, { 1000.0f,600.0f }));
	stageSelectSprite_.reset(Sprite::Create(stageSelectTex_, { 640.0f - 512.0f, 100.0f }));
	stageSelectSprite_->size_ = { 1024.0f,128.0f };
	stageClearSprite_.reset(Sprite::Create(stageClearTex_, { 640.0f - 512.0f, 100.0f }));
	stageClearSprite_->size_ = { 1024.0f,256.0f };
	titleSprite_.reset(Sprite::Create(titleTex_, { 640.0f - 256.0f, 100.0f }));

	stage1Sprite_->size_ = { 128.0f,128.0f };
	stage2Sprite_->size_ = { 128.0f,128.0f };
	stage3Sprite_->size_ = { 128.0f,128.0f };

}

GameScene::~GameScene()
{
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	gv_ = GlobalVariables::GetInstance();
	mapEditor_ = MapEditor::GetInstance();

	mapEditor_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->position_ = { 0.0f,44.0f, -33.0f };
	camera_->rotation_.x = 0.9f;
	
	stage_ = std::make_unique<Stage>();

	stage_->Initialize();

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &camera_->position_.x, 0.1f);
	ImGui::End();


#endif // _DEBUG

	if (!isInitialize_) {

		switch (currentScene_)
		{
		default:
		case GameScene::kTitle:
			TitleInitialize();
			break;
		case GameScene::kSelect:
			SelectInitialize();
			break;
		case GameScene::kGame:
			GameInitialize();
			break;
		}

	}

	switch (currentScene_)
	{
	default:
	case GameScene::kTitle:
		TitleUpdate();
		break;
	case GameScene::kSelect:
		SelectUpdate();
		break;
	case GameScene::kGame:
		GameUpdate();
		break;
	}

	camera_->matRotate_ = MakeRotateMatrix(camera_->rotation_);
	camera_->Update();

}

void GameScene::Draw() {

	skydome_->Draw(camera_.get());

	switch (currentScene_)
	{
	default:
	case GameScene::kTitle:
		TitleDraw();
		break;
	case GameScene::kSelect:
		SelectDraw();
		break;
	case GameScene::kGame:
		GameDraw();
		break;
	}

	if (isSceneChange_) {
		fade_->Draw();
	}

}

void GameScene::TitleInitialize() {

	isInitialize_ = true;

}

void GameScene::TitleUpdate() {

	if (isSceneChange_) {

		SceneChange();

	}
	else {

		if (input_->TriggerButton(XINPUT_GAMEPAD_A)) {
			nextScene_ = kSelect;
			isSceneChange_ = true;
		}

	}

}

void GameScene::TitleDraw() {

	titleSprite_->Draw();

	push_A_Sprite_->Draw();

}

void GameScene::SelectInitialize() {

	isInitialize_ = true;

}

void GameScene::SelectUpdate() {

	if (isSceneChange_) {
		SceneChange();
	}
	else {

		if (input_->TriggerButton(XINPUT_GAMEPAD_START)) {
			nextScene_ = kTitle;
			isSceneChange_ = true;
		}

		if (input_->TriggerButton(XINPUT_GAMEPAD_A)) {
			nextScene_ = kGame;
			isSceneChange_ = true;
		}

		if (input_->TriggerButton(XINPUT_GAMEPAD_DPAD_RIGHT) && stageNumber_ < kMaxStage_) {
			stageNumber_++;
			selectTileSprite_->position_ = { -8.0f + (276.0f * stageNumber_),268.0f };
		}

#ifdef _DEBUG

		if (input_->TriggerButton(XINPUT_GAMEPAD_DPAD_LEFT) && stageNumber_ > 0) {
			stageNumber_--;
			selectTileSprite_->position_ = { -8.0f + (276.0f * stageNumber_),268.0f };
		}

#else

		if (input_->TriggerButton(XINPUT_GAMEPAD_DPAD_LEFT) && stageNumber_ > 1) {
			stageNumber_--;
			selectTileSprite_->position_ = { -8.0f + (276.0f * stageNumber_),268.0f };
		}

#endif // _DEBUG


	}

}

void GameScene::SelectDraw() {

	stage1Sprite_->Draw();

	stage2Sprite_->Draw();

	stage3Sprite_->Draw();

	selectTileSprite_->Draw();

	stageSelectSprite_->Draw();

	selectSprite_->Draw();

	push_A_Sprite_->Draw();

}

void GameScene::GameInitialize() {


	if (stageNumber_ != 0) {

		std::string loadName = "stage";

		loadName += std::to_string(stageNumber_);

		stage_->Initialize();
		stage_->LoadStage(loadName);
	}

	camera_->position_ = { 0.0f,44.0f, -33.0f };
	camera_->rotation_.x = 0.9f;

	isClear_ = false;

	isInitialize_ = true;

}

void GameScene::GameUpdate() {

	if (isSceneChange_) {
		SceneChange();
	}
	else if(!isClear_) {

		if (input_->TriggerButton(XINPUT_GAMEPAD_START)) {
			nextScene_ = kSelect;
			isSceneChange_ = true;
		}

		if (stage_->GetAllBlockRock()) {
			isClear_ = true;
		}

	}
	else {

		if (input_->TriggerButton(XINPUT_GAMEPAD_A)) {
			nextScene_ = kSelect;
			isSceneChange_ = true;
		}

	}

	if (stageNumber_ == 0) {
		mapEditor_->Edit();
	}
	else {
		stage_->Update();
	}

}

void GameScene::GameDraw() {

	if (stageNumber_ == 0) {
		mapEditor_->Draw(camera_.get());
	}
	else {
		stage_->Draw(camera_.get());
	}

	if (stageNumber_ != 0) {
		stage_->DrawParticle(camera_.get());
	}

	if (isClear_) {
		stageClearSprite_->Draw();
		push_A_Sprite_->Draw();
	}

	moveSprite_->Draw();

}

void GameScene::SceneChange() {

	if (currentScene_ != nextScene_) {

		if (fade_->color_.w < 1.0f) {
			fade_->color_.w += 1.0f / 30.0f;

			if (fade_->color_.w >= 1.0f) {
				fade_->color_.w = 1.0f;
				currentScene_ = nextScene_;
				isInitialize_ = false;
			}

		}

	}
	else {

		if (fade_->color_.w > 0.0f) {
			fade_->color_.w -= 1.0f / 30.0f;

			if (fade_->color_.w <= 0.0f) {
				fade_->color_.w = 0.0f;
				isSceneChange_ = false;
			}

		}

	}

}
