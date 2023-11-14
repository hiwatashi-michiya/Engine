#pragma once
#include <list>
#include "Engine/3d/Model.h"
#include "Engine/2d/Sprite.h"


//横最大幅
const int kMaxWidth = 20;
//縦最大幅
const int kMaxHeight = 20;

//奥行き最大幅
const int kMaxDepth = 10;

//ブロックの種類
const int kMaxBlockType = 5;

//リストの最大サイズ(Undo, Redo)
const int kMaxListSize = 1200;

//範囲塗りつぶしリストの最大サイズ
const int kMaxFillListSIze = kMaxWidth * kMaxHeight * kMaxListSize;

enum BlockType {
	NONE,
	GROUND,
	BLOCK,
	WONDERBLOCK,
	FIXEDBLOCK
};

enum TOOL
{
	BRUSH,
	RANGEFILL,
	SELECT
};

//マップの操作を行うクラス
class Map
{
public:
	Map();
	~Map();

	//更新
	void Update();
	//描画
	void Draw();
	//csvロード
	void Load();
	//csvセーブ
	void Save();

private:

	//書き換え機能
	void Edit();

	//配列の数字によってブロックの持つ情報を変える
	void SetState(int mapNum);

	//Undo
	void Undo();

	//Redo
	void Redo();

	//範囲塗りつぶし
	void RangeFill();

	//範囲選択
	void Select();

	//csv書き換えに使う文字列
	char string[kMaxBlockType][2] = { "0", "1", "2", "3", "4" };

	//書き換える配列の要素を格納するリスト。mapの要素、要素の行数、要素の列数の順に格納する
	std::list<int> undoArrayList;

	//手戻りしたときの要素保管場所
	std::list<int> redoArrayList;

	//範囲選択時の同時塗りつぶし(undo)
	std::list<int> undoFillArrayList;

	//範囲選択時の同時塗りつぶし(redo)
	std::list<int> redoFillArrayList;

	//コピーリスト
	std::list<int> copyArrayList;

	//リストの要素を参照する変数
	int tmpArrayX;
	int tmpArrayY;
	int tmpArrayZ;
	int tmpArrayType;

	//マップチップの数
	int map[kMaxDepth][kMaxHeight][kMaxWidth];

	//マップチップ上の位置X
	int XGrid;
	//マップチップ上の位置Y
	int YGrid;
	//マップチップ上の位置Z
	int ZGrid;

	//設置するブロックのナンバー
	int blockNum;

	//マップ書き換え可能かどうか
	bool isEdit;

	//範囲塗りつぶしを行うかどうか
	bool isRangeFill;

	//範囲塗りつぶし始点座標X
	int startRangeFillX;
	//範囲塗りつぶし終点座標X
	int endRangeFillX;
	//範囲塗りつぶし始点座標Y
	int startRangeFillY;
	//範囲塗りつぶし終点座標Y
	int endRangeFillY;
	//範囲塗りつぶし始点座標Z
	int startRangeFillZ;
	//範囲塗りつぶし終点座標Z
	int endRangeFillZ;

	//画面外の表示を制限
	int borderRight;
	int borderLeft;
	int borderTop;
	int borderDown;

	//画像
	Texture textureHandle;
	Texture frameTexture;

	//色
	int color;

	//キー入力を管理するためのカウント
	int keyCount;
	const int kMaxKeyCount = 15;

	//値を範囲内に収める関数
	int Clamp(int x, int min, int max);

	//ImGuiの位置、サイズを格納する変数
	float ImGuiPosX;
	float ImGuiPosY;
	float ImGuiWidth;
	float ImGuiHeight;

	//現在選択しているマップの場所を表示するための座標
	int selectX;
	int selectY;

	//その他画像
	Texture bgTexture;
	Texture groundTexture;
	Texture blockTexture;
	Texture wonderBlockTexture;
	Texture fixedBlockTexture;

	//現在の機能
	TOOL tool;
	char toolString[3][10] = { "Brush","RangeFill", "Select" };

	//範囲選択しているかどうか
	bool isSelect;

	//セーブしたかどうか
	bool isSave;

};


