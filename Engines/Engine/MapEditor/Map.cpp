#include "Map.h"
#include <stdio.h>
#include <cassert>
#include "Engine/input/Input.h"
#include "Engine/manager/ImGuiManager.h"

Map::Map()
{

	Load();

	XGrid = 0;
	YGrid = 0;

	tmpArrayX = 0;
	tmpArrayY = 0;
	tmpArrayType = 0;

	blockNum = 0;

	isEdit = false;
	isRangeFill = false;
	isSelect = false;
	isSave = true;

	startRangeFillX = 0;
	endRangeFillX = 0;
	startRangeFillY = 0;
	endRangeFillY = 0;


	ImGuiPosX = 0;
	ImGuiPosY = 0;
	ImGuiWidth = 0;
	ImGuiHeight = 0;

	selectX = 0;
	selectY = 0;

	keyCount = kMaxKeyCount;

	tool = BRUSH;

	textureHandle = TextureManager::GetInstance()->Load("./resources/Texture/block.png");
	frameTexture = TextureManager::GetInstance()->Load("./resources/Texture/frameborder.png");
	bgTexture = TextureManager::GetInstance()->Load("./resources/Texture/background.png");
	groundTexture = TextureManager::GetInstance()->Load("./resources/Texture/ground.png");
	blockTexture = TextureManager::GetInstance()->Load("./resources/Texture/block.png");
	wonderBlockTexture = TextureManager::GetInstance()->Load("./resources/Texture/wonderblock.png");
	fixedBlockTexture = TextureManager::GetInstance()->Load("./resources/Texture/fixedblock.png");

	color = 0xFFFFFFFF;

}

Map::~Map()
{
}

void Map::Update() {

	ImGui::Begin("Editor");
	ImGuiPosX = ImGui::GetWindowPos().x;
	ImGuiPosY = ImGui::GetWindowPos().y;
	ImGuiWidth = ImGui::GetWindowSize().x;
	ImGuiHeight = ImGui::GetWindowSize().y;
	ImGui::DragInt("blockNum", &blockNum, 0.05f, 0, kMaxBlockType - 1);
	ImGui::InputText("tool", &toolString[tool][0], ImGuiInputTextFlags_ReadOnly);
	ImGui::End();

	Edit();

}

void Map::Draw() {

	for (int z = 0; z < kMaxDepth; z++) {
		for (int y = 0; y < kMaxHeight; y++) {
			for (int x = 0; x < kMaxWidth; x++) {

				SetState(map[z][y][x]);

				//ブロックがあったら表示
				if (map[z][y][x] != NONE) {
					Novice::DrawQuad(x * kMapChipSize, y * kMapChipSize,
						x * kMapChipSize + kMapChipSize, y * kMapChipSize,
						x * kMapChipSize, y * kMapChipSize + kMapChipSize,
						x * kMapChipSize + kMapChipSize, y * kMapChipSize + kMapChipSize,
						0, 0, 32, 32, textureHandle, color);
				}
			}
		}
	}


	if (isEdit) {

		/*if (Novice::IsPressMouse(0)) {
			Novice::DrawEllipse(mouseX, mouseY, 5, 5, 0.0f, 0xFF0000FF, kFillModeSolid);
		}
		else {
			Novice::DrawEllipse(mouseX, mouseY, 5, 5, 0.0f, 0xFFFFFFFF, kFillModeSolid);
		}*/

		/*Novice::ScreenPrintf(0, 0, "undoArrayList size : %d", undoArrayList.size());
		Novice::ScreenPrintf(0, 20, "redoArrayList size : %d", redoArrayList.size());
		Novice::ScreenPrintf(0, 40, "blockNumber : %d", blockNum);*/

	}

}

void Map::Edit() {

	//書き換えオンオフ設定
	if (Input::GetInstance()->TriggerKey(DIK_E)) {

		if (isEdit) {
			isEdit = false;
		}
		else {
			isEdit = true;
		}

	}

	if (isEdit == true) {

		//ブロック切り替え
		if (Input::GetInstance()->TriggerKey(DIK_W)) {

			if (blockNum < kMaxBlockType - 1) {
				blockNum++;
			}

		}

		if (Input::GetInstance()->TriggerKey(DIK_S)) {

			if (blockNum > 0) {
				blockNum--;
			}

		}

		//機能切り替え
		if (Input::GetInstance()->TriggerKey(DIK_1)) {
			tool = BRUSH;
		}
		else if (Input::GetInstance()->TriggerKey(DIK_2)) {
			tool = RANGEFILL;
		}
		else if (Input::GetInstance()->TriggerKey(DIK_3)) {
			tool = SELECT;
		}

		//ImGuiウィンドウの範囲内を反応させない
		/*if (!(ImGuiPosX <= mouseX && mouseX <= ImGuiPosX + ImGuiWidth &&
			ImGuiPosY <= mouseY && mouseY <= ImGuiPosY + ImGuiHeight)) {

			

		}*/

		//スペースを押した場合
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {

			//現在の機能によって行うことを変える
			switch (tool)
			{
			case BRUSH:
			default:

				//単選択

				//配列外参照を起こさない
				XGrid = Clamp(XGrid, 0, kMaxWidth);
				YGrid = Clamp(YGrid, 0, kMaxHeight);

				//書き換える前と後の要素が同じ場合スルー
				if (map[ZGrid][YGrid][XGrid] != blockNum) {

					//未セーブ状態を知らせる
					if (isSave) {
						isSave = false;
					}

					//redoのリストに要素があった場合、空にする
					if (redoArrayList.empty() != true) {
						redoArrayList.clear();
					}

					//redoのリスト(範囲塗りつぶし)に要素があった場合、空にする
					if (redoFillArrayList.empty() != true) {
						redoFillArrayList.clear();
					}

					//リストの最後尾に変更前の要素を追加
					//マップのナンバーを最初に格納
					undoArrayList.push_back(map[ZGrid][YGrid][XGrid]);
					//行の数字を追加
					undoArrayList.push_back(YGrid);
					//列の数字を追加
					undoArrayList.push_back(XGrid);

					//サイズが一定値を超えたら古い順に削除
					if (undoArrayList.size() > kMaxListSize) {
						for (int i = 0; i < 3; i++) {
							undoArrayList.pop_front();
						}
					}

					//配列の要素を変更
					map[ZGrid][YGrid][XGrid] = blockNum;

				}

				break;
			case RANGEFILL:

				//範囲塗りつぶし
				if (isRangeFill == false) {

					//フラグを立たせる
					isRangeFill = true;
				}



				break;
			case SELECT:

				//範囲選択
				if (isSelect == false) {
				
					isSelect = true;
				}



				break;

			}

		}

		//範囲塗りつぶし
		if (isRangeFill && !Input::GetInstance()->PushKey(DIK_SPACE) && tool == RANGEFILL) {

			//未セーブ状態を知らせる
			if (isSave) {
				isSave = false;
			}

			RangeFill();
			
		}

		if (isSelect && !Input::GetInstance()->PushKey(DIK_SPACE) && tool == SELECT) {

			//未セーブ状態を知らせる
			if (isSave) {
				isSave = false;
			}

			Select();

		}

		if (Input::GetInstance()->PushKey(DIK_LCONTROL)) {

			//ctrl + Zで手戻り、ctrl + Yで元に戻す
			if (Input::GetInstance()->PushKey(DIK_Z)) {

				//操作性向上の為に入力を管理
				if (keyCount == kMaxKeyCount || (keyCount < 3 && keyCount % 3 == 0)) {
					Undo();
				}

				if (keyCount == 0) {
					keyCount = 3;
				}

				keyCount--;

			}
			else if (Input::GetInstance()->PushKey(DIK_Y)) {

				//操作性向上の為に入力を管理
				if (keyCount == kMaxKeyCount || (keyCount < 3 && keyCount % 3 == 0)) {
					Redo();
				}

				if (keyCount == 0) {
					keyCount = 3;
				}

				keyCount--;

			}
			
			//ctrl + S でセーブ
			if (Input::GetInstance()->TriggerKey(DIK_S)) {
				Save();
			}

		}

		//Z、Yどっちも押さずキーカウントが最大値でなければキーカウントリセット
		if (keyCount != kMaxKeyCount && !Input::GetInstance()->PushKey(DIK_Z) && !Input::GetInstance()->PushKey(DIK_Y)) {
			keyCount = kMaxKeyCount;
		}

	}

}

//マップチップのステータス設定
void Map::SetState(int mapNum) {

	switch (mapNum)
	{
	case NONE:
	default:
		color = 0xFFFFFFFF;
		break;
	case GROUND:
		color = 0xFFFFFFFF;
		textureHandle = groundTexture;
		break;
	case BLOCK:
		color = 0xFFFFFFFF;
		textureHandle = blockTexture;
		break;
	case WONDERBLOCK:
		color = 0xFFFFFFFF;
		textureHandle = wonderBlockTexture;
		break;
	case FIXEDBLOCK:
		color = 0xFFFFFFFF;
		textureHandle = fixedBlockTexture;
		break;
	}

}

//マップの読み込み
void Map::Load() {

	FILE* fp = NULL;

	fopen_s(&fp, "./Resources/Maps/map.csv", "rb");

	assert(fp != NULL);

	for (int y = 0; y < kMaxHeight; y++) {
		for (int x = 0; x < kMaxWidth; x++) {
			fscanf_s(fp, "%d,", &map[y][x]);
		}
	}

	fclose(fp);

}

//範囲塗りつぶし
void Map::RangeFill() {

	//塗りつぶし終了座標を決める
	endRangeFillX = XGrid;
	endRangeFillY = YGrid;

	//塗りつぶし範囲の左右、上下
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;

	//開始座標と終了座標のどっちが左かを確認する
	if (startRangeFillX <= endRangeFillX) {
		left = startRangeFillX;
		right = endRangeFillX;
	}
	else {
		left = endRangeFillX;
		right = startRangeFillX;
	}

	//開始座標と終了座標のどっちが上かを確認する
	if (startRangeFillY <= endRangeFillY) {
		top = startRangeFillY;
		bottom = endRangeFillY;
	}
	else {
		top = endRangeFillY;
		bottom = startRangeFillY;
	}

	//配列外参照にならないよう値を収める
	left = Clamp(left, 0, kMaxWidth - 1);
	right = Clamp(right, 0, kMaxWidth - 1);
	top = Clamp(top, 0, kMaxHeight - 1);
	bottom = Clamp(bottom, 0, kMaxHeight - 1);

	//範囲内塗りつぶしを開始する
	for (int y = top; y < bottom + 1; y++) {
		for (int x = left; x < right + 1; x++) {

			//redoのリストに要素があった場合、空にする
			if (redoArrayList.empty() != true) {
				redoArrayList.clear();
			}

			//redoのリスト(範囲塗りつぶし)に要素があった場合、空にする
			if (redoFillArrayList.empty() != true) {
				redoFillArrayList.clear();
			}

			//リストの最後尾に変更前の要素を追加
			//マップのナンバーを格納
			undoFillArrayList.push_back(map[z][y][x]);

			//サイズが一定値を超えたら古い順に削除
			if (undoFillArrayList.size() > kMaxFillListSIze) {
				for (int i = 0; i < 3; i++) {
					undoFillArrayList.pop_front();
				}
			}

			//配列の要素を変更
			map[y][x] = blockNum;

		}
	}

	//範囲を格納
	undoFillArrayList.push_back(left);
	undoFillArrayList.push_back(right);
	undoFillArrayList.push_back(top);
	undoFillArrayList.push_back(bottom);

	//範囲塗りつぶし用の値をundoリストに格納。値がずれないように三回行う
	for (int i = 0; i < 3; i++) {
		undoArrayList.push_back(-1);
	}

	//フラグを下げる
	isRangeFill = false;

}

//セーブ機能
void Map::Save() {

	FILE* fp = NULL;

	fopen_s(&fp, "./Resources/Maps/map.csv", "r+b");

	assert(fp != NULL);

	for (int z = 0; z < kMaxDepth; z++) {
		for (int y = 0; y < kMaxHeight; y++) {
			for (int x = 0; x < kMaxWidth; x++) {
				//ファイルの書き込む場所を探す
				fseek(fp, (y * (kMaxWidth * 2 + 2)) + (x * 2), SEEK_SET);
				//指定した場所の値を書き換える
				fputs(string[map[z][y][x]], fp);
			}
		}
	}
	

	fclose(fp);

	//セーブが完了したことを伝える
	isSave = true;

}

//手戻り
void Map::Undo() {

	//リストが空でないときに処理
	if (undoArrayList.empty() != true) {

		//範囲塗りつぶしをしていたかどうか
		if (undoArrayList.back() != -1) {

			//要素を一つずつ取り出して削除、現在のマップを保管用リストに追加
			tmpArrayX = undoArrayList.back();
			undoArrayList.pop_back();
			tmpArrayY = undoArrayList.back();
			undoArrayList.pop_back();
			tmpArrayZ = undoArrayList.back();

			tmpArrayType = undoArrayList.back();
			undoArrayList.pop_back();

			redoArrayList.push_back(tmpArrayX);
			redoArrayList.push_back(tmpArrayY);
			redoArrayList.push_back(map[tmpArrayY][tmpArrayX]);

			//取り出した要素を使い書き換える前のマップに戻す
			map[tmpArrayY][tmpArrayX] = tmpArrayType;

		}
		//範囲塗りつぶしの場合の処理
		else {

			//undoリストから値を削除
			for (int i = 0; i < 3; i++) {
				undoArrayList.pop_back();
			}

			//範囲の要素を取り出し代入
			int bottom = undoFillArrayList.back();
			undoFillArrayList.pop_back();
			int top = undoFillArrayList.back();
			undoFillArrayList.pop_back();
			int right = undoFillArrayList.back();
			undoFillArrayList.pop_back();
			int left = undoFillArrayList.back();
			undoFillArrayList.pop_back();

			//塗った時と逆の手順で塗りつぶしを行う
			for (int y = bottom; y >= top; y--) {
				for (int x = right; x >= left; x--) {
					//タイプを格納
					redoFillArrayList.push_back(map[y][x]);
					//要素を代入
					map[y][x] = undoFillArrayList.back();
					//undo塗りつぶしリストの要素を削除
					undoFillArrayList.pop_back();
				}
			}

			//範囲を格納
			redoFillArrayList.push_back(left);
			redoFillArrayList.push_back(right);
			redoFillArrayList.push_back(top);
			redoFillArrayList.push_back(bottom);

			//範囲塗りつぶし用の値をredoリストに格納。値がずれないように三回行う
			for (int i = 0; i < 3; i++) {
				redoArrayList.push_back(-1);
			}

		}

		

	}

}

//元に戻す
void Map::Redo() {

	//リストが空でないときに処理
	if (redoArrayList.empty() != true) {

		//範囲塗りつぶしをしていたかどうか
		if (redoArrayList.back() != -1) {

			//保管用のリストから要素を取り出して元のリストに戻し削除
			tmpArrayType = redoArrayList.back();
			redoArrayList.pop_back();
			tmpArrayY = redoArrayList.back();
			redoArrayList.pop_back();
			tmpArrayX = redoArrayList.back();
			redoArrayList.pop_back();

			undoArrayList.push_back(map[tmpArrayY][tmpArrayX]);
			undoArrayList.push_back(tmpArrayY);
			undoArrayList.push_back(tmpArrayX);

			//取り出した要素を使い元のマップに戻す
			map[tmpArrayY][tmpArrayX] = tmpArrayType;

		}
		else {

			//redoリストから値を削除
			for (int i = 0; i < 3; i++) {
				redoArrayList.pop_back();
			}

			//範囲の要素を取り出し代入
			int bottom = redoFillArrayList.back();
			redoFillArrayList.pop_back();
			int top = redoFillArrayList.back();
			redoFillArrayList.pop_back();
			int right = redoFillArrayList.back();
			redoFillArrayList.pop_back();
			int left = redoFillArrayList.back();
			redoFillArrayList.pop_back();

			for (int y = top; y < bottom + 1; y++) {
				for (int x = left; x < right + 1; x++) {
					//タイプを格納
					undoFillArrayList.push_back(map[y][x]);
					//要素を代入
					map[y][x] = redoFillArrayList.back();
					//undo塗りつぶしリストの要素を削除
					redoFillArrayList.pop_back();
				}
			}

			//範囲を格納
			undoFillArrayList.push_back(left);
			undoFillArrayList.push_back(right);
			undoFillArrayList.push_back(top);
			undoFillArrayList.push_back(bottom);

			//範囲塗りつぶし用の値をundoリストに格納。値がずれないように三回行う
			for (int i = 0; i < 3; i++) {
				undoArrayList.push_back(-1);
			}

		}

		

	}

}

void Map::Select() {

	isSelect = false;

}

int Map::Clamp(int x, int min, int max) {

	if (x < min) {
		x = min;
	}

	if (x > max) {
		x = max;
	}

	return x;

}
