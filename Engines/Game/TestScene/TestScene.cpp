#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TestScene.h"
#include "FrameWork/SceneManager.h"
#include "PostEffectDrawer.h"
#include <cmath>

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "winmm.lib")


#ifdef _DEBUG

#include "Drawing/ImGuiManager.h"

#endif // _DEBUG

NT_Player TestScene::cPlayer_, TestScene::sPlayer_;
std::array<std::unique_ptr<Item>, TestScene::kMaxItem_> TestScene::items_;

TestScene::TestScene()
{

}

TestScene::~TestScene()
{
}

void TestScene::Initialize() {

	dxSetter_ = DirectXSetter::GetInstance();
	input_ = Input::GetInstance();
	audioManager_ = AudioManager::GetInstance();

	WSADATA wsaData;

	cPlayer_.position = { 1180.0f,360.0f };
	cPlayer_.count = 0;
	sPlayer_.position = { 100.0f,360.0f };
	sPlayer_.count = 0;

	playerTex_ = TextureManager::GetInstance()->Load("./Resources/textures/player.png");
	numTex_ = TextureManager::GetInstance()->Load("./Resources/UI/num.png");
	winTex_ = TextureManager::GetInstance()->Load("./Resources/textures/win.png");
	loseTex_ = TextureManager::GetInstance()->Load("./Resources/textures/lose.png");
	bgTex_ = TextureManager::GetInstance()->Load("./Resources/white.png");

	cPlayerSprite_.reset(Sprite::Create(playerTex_, cPlayer_.position, { 0.5f,0.5f,1.0f,1.0f }));
	sPlayerSprite_.reset(Sprite::Create(playerTex_, sPlayer_.position, { 1.0f,0.5f,0.5f,1.0f }));
	cPlayerSprite_->anchorPoint_ = { 0.5f,0.5f };
	sPlayerSprite_->anchorPoint_ = { 0.5f,0.5f };
	cPlayerSprite_->size_ = { 64.0f,64.0f };
	sPlayerSprite_->size_ = { 64.0f,64.0f };

	winSprite_.reset(Sprite::Create(winTex_, { 640.0f,360.0f }, { 1.0f,0.0f,0.0f,1.0f }));
	loseSprite_.reset(Sprite::Create(loseTex_, { 640.0f,360.0f }, { 0.0f,0.0f,1.0f,1.0f }));
	winSprite_->anchorPoint_ = { 0.5f,0.5f };
	winSprite_->size_ = { 512.0f,128.0f };
	loseSprite_->anchorPoint_ = { 0.5f,0.5f };
	loseSprite_->size_ = { 512.0f,128.0f };

	bgSprite_.reset(Sprite::Create(bgTex_, {}, { 0.0f,0.0f,0.0f,1.0f }));
	bgSprite_->size_ = { 1280.0f,720.0f };

	for (int32_t i = 0; i < 2; i++) {

		cCountSprite_[i].reset(Sprite::Create(numTex_, { 1100.0f + 48.0f * i, 28.0f }));
		cCountSprite_[i]->size_ = { 64.0f,64.0f };
		cCountSprite_[i]->viewRect_ = { 0.1f,1.0f };
		cCountSprite_[i]->color_ = { 0.5f,0.5f,1.0f,1.0f };

		sCountSprite_[i].reset(Sprite::Create(numTex_, { 68.0f + 48.0f * i, 28.0f }));
		sCountSprite_[i]->size_ = { 64.0f,64.0f };
		sCountSprite_[i]->viewRect_ = { 0.1f,1.0f };
		sCountSprite_[i]->color_ = { 1.0f,0.5f,0.5f,1.0f };

	}

	//winsock初期化
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	static HANDLE hThread;
	static DWORD dwID;

	if (isServer_) {
		hThread = (HANDLE)CreateThread(NULL, 0, &SThreadfunc, (LPVOID)&sPlayer_, 0, &dwID);
	}
	else {
		hThread = (HANDLE)CreateThread(NULL, 0, &CThreadfunc, (LPVOID)&cPlayer_, 0, &dwID);
	}

	for (int32_t i = 0; i < kMaxItem_; i++) {

		items_[i] = std::make_unique<Item>();
		items_[i]->Initialize();
		items_[i]->SetPosition({ 340.0f + 100.0f * (i % 7), 60.0f + 100.0f * (i / 7) });

	}

}

void TestScene::Finalize()
{
}

void TestScene::Update() {

#ifdef _DEBUG

	

#endif // _DEBUG
	
	if (isGetAll_) {
		isEnd_ = true;
	}

	if (isEnd_) {

		if (input_->TriggerKey(DIK_R) or input_->TriggerKey(DIK_SPACE)) {
			SceneChangeManager::GetInstance()->SetNextScene("TEST");
			SceneChangeManager::GetInstance()->SceneChangeStart();
		}

		if (isServer_) {

			if (sPlayer_.count >= cPlayer_.count) {
				isWin_ = true;
			}
			else {
				isWin_ = false;
			}

		}
		else {

			if (cPlayer_.count >= sPlayer_.count) {
				isWin_ = true;
			}
			else {
				isWin_ = false;
			}

		}

		return;

	}

	if (isServer_) {

		if (input_->PushKey(DIK_W)) {
			sPlayer_.position.y -= 5.0f;
		}
		else if (input_->PushKey(DIK_S)) {
			sPlayer_.position.y += 5.0f;
		}

		if (input_->PushKey(DIK_A)) {
			sPlayer_.position.x -= 5.0f;
		}
		else if (input_->PushKey(DIK_D)) {
			sPlayer_.position.x += 5.0f;
		}

	}
	else {

		if (input_->PushKey(DIK_UP)) {
			cPlayer_.position.y -= 5.0f;
		}
		else if (input_->PushKey(DIK_DOWN)) {
			cPlayer_.position.y += 5.0f;
		}

		if (input_->PushKey(DIK_LEFT)) {
			cPlayer_.position.x -= 5.0f;
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			cPlayer_.position.x += 5.0f;
		}

	}

	sPlayerSprite_->position_ = sPlayer_.position;
	cPlayerSprite_->position_ = cPlayer_.position;

	isGetAll_ = true;

	for (int32_t i = 0; i < kMaxItem_; i++) {

		if (not items_[i]->GetIsActive()) {
			continue;
		}

		items_[i]->Update();

		if (items_[i]->CheckCollision(cPlayer_.position, { 64.0f,64.0f })) {
			items_[i]->SetIsActive(false);
			cPlayer_.count++;
		}

		if (items_[i]->CheckCollision(sPlayer_.position, { 64.0f,64.0f })) {
			items_[i]->SetIsActive(false);
			sPlayer_.count++;
		}

		isGetAll_ = false;

	}

	//10分割するために定義
	float division = 0.1f;

	//割る数字
	int32_t divideNum = int32_t(std::pow(10, 2 - 1));

	//現在の数字
	int32_t cCurrentNum = cPlayer_.count;
	int32_t sCurrentNum = sPlayer_.count;

	for (int32_t i = 0; i < 2; i++) {

		if (divideNum == 0) {
			break;
		}

		//描画する数字
		int32_t cSetNum = cCurrentNum / divideNum;

		//上の方の桁から計算し、UVセット
		cCountSprite_[i]->uvTranslate_.x = float(cSetNum) * division;

		//描画する数字
		int32_t sSetNum = sCurrentNum / divideNum;

		//上の方の桁から計算し、UVセット
		sCountSprite_[i]->uvTranslate_.x = float(sSetNum) * division;

		//次の桁に移行する
		cCurrentNum = cCurrentNum % divideNum;
		sCurrentNum = sCurrentNum % divideNum;
		divideNum = int32_t(divideNum * 0.1f);

	}

}

void TestScene::Draw()
{

	bgSprite_->Draw();

	for (int32_t i = 0; i < kMaxItem_; i++) {

		items_[i]->Draw();

	}

	cPlayerSprite_->Draw();
	sPlayerSprite_->Draw();

	if (isEnd_) {

		if (isWin_) {
			winSprite_->Draw();
		}
		else {
			loseSprite_->Draw();
		}

	}

	for (int32_t i = 0; i < 2; i++) {
		sCountSprite_[i]->Draw();
		cCountSprite_[i]->Draw();
	}

}

DWORD WINAPI TestScene::SThreadfunc([[maybe_unused]] void* px)
{

	SOCKET sWait, sConnect;
	WORD wPort = 8000;
	SOCKADDR_IN saConnect, saLocal;
	int iLen, iRecv;

	//リッスンソケット
	sWait = socket(AF_INET, SOCK_STREAM, 0);

	ZeroMemory(&saLocal, sizeof(saLocal));

	//8000番に接続待機用ソケット作成
	saLocal.sin_family = AF_INET;
	saLocal.sin_port = htons(wPort);
	saLocal.sin_addr.s_addr = INADDR_ANY;

	if (bind(sWait, reinterpret_cast<SOCKADDR*>(&saLocal), sizeof(SOCKADDR)) == SOCKET_ERROR) {

		//ここでエラー
		closesocket(sWait);
		WSACleanup();
		return 1;

	}

	if (listen(sWait, 2) == SOCKET_ERROR) {

		closesocket(sWait);
		WSACleanup();
		return 1;

	}

	iLen = sizeof(saConnect);

	//接続受け入れて送受信用ソケット作成
	sConnect = accept(sWait, reinterpret_cast<SOCKADDR*>(&saConnect), &iLen);

	//接続待ち用ソケット解放
	closesocket(sWait);

	if (sConnect == INVALID_SOCKET) {

		shutdown(sConnect, 2);
		closesocket(sConnect);
		shutdown(sWait, 2);
		closesocket(sWait);
		WSACleanup();

		return 1;

	}

	iRecv = 0;

	while (1)
	{

		int nRcv;

		//クライアント側キャラの位置情報を受け取り
		nRcv = recv(sConnect, (char*)(&cPlayer_), sizeof(NT_Player), 0);

		if (nRcv == SOCKET_ERROR) {
			break;
		}

		//サーバ側キャラの位置情報を送信
		send(sConnect, (char*)(&sPlayer_), sizeof(NT_Player), 0);

	}

	shutdown(sConnect, 2);
	closesocket(sConnect);
	WSACleanup();


	return 0;
}

DWORD __stdcall TestScene::CThreadfunc(void*)
{
	
	SOCKET sConnect;
	WORD wPort = 8000;
	SOCKADDR_IN saConnect;
	int iRecv;
	char szServer[1024] = "127.0.0.1";
	unsigned int addr;

	//ソケットを開く
	sConnect = socket(AF_INET, SOCK_STREAM, 0);

	if (sConnect == INVALID_SOCKET) {

		shutdown(sConnect, 2);
		closesocket(sConnect);

		return 1;

	}

	//サーバーを名前で取得する
	HOSTENT* lpHost;

	lpHost = gethostbyname(szServer);

	if (lpHost == NULL) {
		//サーバーをIPアドレスで取得
		addr = inet_addr(szServer);
		lpHost = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	//クライアントソケットをサーバーに接続
	memset(&saConnect, 0, sizeof(SOCKADDR_IN));
	saConnect.sin_family = lpHost->h_addrtype;
	saConnect.sin_port = htons(wPort);
	saConnect.sin_addr.s_addr = *((u_long*)lpHost->h_addr);

	if (connect(sConnect, (SOCKADDR*)&saConnect, sizeof(saConnect)) != SOCKET_ERROR) {

	}
	else {

		//失敗した場合
		shutdown(sConnect, 2);
		closesocket(sConnect);
		return 1;

	}

	iRecv = 0;

	while (1)
	{

		//クライアント側キャラの位置情報を送信
		send(sConnect, (const char*)(&cPlayer_), sizeof(NT_Player), 0);

		int nRcv;

		//サーバ側キャラの位置情報を受け取り
		nRcv = recv(sConnect, (char*)(&sPlayer_), sizeof(NT_Player), 0);

		if (nRcv == SOCKET_ERROR) {
			break;
		}

	}

	shutdown(sConnect, 2);
	closesocket(sConnect);

	return 0;

}

void Item::Initialize()
{

	tex_ = TextureManager::GetInstance()->Load("./Resources/white.png");
	item_.reset(Sprite::Create(tex_, position_, { 1.0f,1.0f,0.0f,1.0f }));
	item_->size_ = { 32.0f,32.0f };
	item_->anchorPoint_ = { 0.5f,0.5f };

}

void Item::Update()
{

	item_->position_ = position_;

}

void Item::Draw()
{

	if (isActive_) {
		item_->Draw();
	}

}

bool Item::CheckCollision(const Vector2& position, const Vector2& size)
{

	if ((position_.x - 16.0f < position.x + size.x * 0.5f and position_.x + 16.0f > position.x - size.x * 0.5f) and
		(position_.y - 16.0f < position.y + size.y * 0.5f and position_.y + 16.0f > position.y - size.y * 0.5f)) {
		return true;
	}

	return false;

}
