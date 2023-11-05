#include <string>
using namespace std;

#include "Common.h"
#include "Mover.h"
#include "..\..\LibUtil\LibUtil.h"

//==============================================================
// ゲーム本体
class CBulletGame : public CGame {
	bool Paused;
	bool PrevInput;
	CFont* Font;
	int StageIndex;
	bool StageActive;
	int Slow;
	int Time;
	bool SlowUsed;
	int TopTime;
public:
	CBulletGame();
	~CBulletGame();
	
	virtual void OnResetDevice();
	virtual void Move();
	virtual void Draw();

	void SetStage(int index);
	void MoveTask(CTaskList* list);
	void DrawTask(CTaskList* list);
	void DeleteTask(CTaskList* list);
};

// コンストラクタ
CBulletGame::CBulletGame()
:	CGame(L"書籍『弾幕　最強のシューティングゲームを作る！』（松浦健一郎／司ゆき著）サンプルプログラム", true, false, true), 
	Paused(false), PrevInput(false),
	StageIndex(0), StageActive(false),
	Slow(1), Time(0), 
	SlowUsed(0), TopTime(0)
{
	// グローバル変数の初期化
	::Graphics=GetGraphics();
	ShowHit=false;
	ShowColor=false;

	// ヘルプ、終了、カーソルに関する設定
	HelpURL=ExePath+L"..\\Help\\index.html";
	ConfirmExit=false;
	ShowCursor(TRUE);
	FPS=60;
	PauseInTheBackground=true;
	DropFrames=false;

	// フォントの初期化
	const wchar_t fontchar[]=L"~|0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.(){}<>!?+-x*/='\\\"#$%&@[]";
	Font=new CFont(Graphics->GetDevice(), ExePath+L"..\\Font\\font.png", NULL, fontchar, 16.0f/256, 32.0f/256, 16, 32, 2, 2);

	// シェイプの初期化
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();
	for (size_t i=0; i<ShapeCount; i++) {
		wstring path;
		path=ExePath+L"..\\Texture\\"+ShapeList[i].TextureName+L"M.png";
		ShapeList[i].Texture[0]=new CTexture(device, path, NULL);
		path=ExePath+L"..\\Texture\\"+ShapeList[i].TextureName+L"C.png";
		ShapeList[i].Texture[1]=new CTexture(device, path, NULL);
	}
	
	// タスクリストの初期化
	MyShipList=new CTaskList(100, 5);
	EnemyList=new CTaskList(100, 50);
	BulletList=new CTaskList(100, 5000);

	// 設定ファイル
	CStrings s;
	s.LoadFromFile(ExePath+L"..\\Bullet.ini");
	ShowHit=(s.GetValue(L"ShowHit")==L"true");
	ShowColor=(s.GetValue(L"ShowColor")==L"true");
	Slow=min(8, max(1, ToInt(s.GetValue(L"Slow"))));
	for (size_t i=0; i<StageCount; i++) {
		wstring t=s.GetValue(wstring(L"TopTime")+ToStr(i));
		Stage[i].TopTime=(t!=L"")?ToInt(t):0;
	}
}

// デストラクタ
CBulletGame::~CBulletGame() {
	CStrings s;
	s.SetValue(L"ShowHit", ShowHit?L"true":L"false");
	s.SetValue(L"ShowColor", ShowColor?L"true":L"false");
	s.SetValue(L"Slow", ToStr(Slow));
	for (size_t i=0; i<StageCount; i++) {
		s.SetValue(wstring(L"TopTime")+ToStr(i), Stage[i].TopTime);
	}
	s.SaveToFile(ExePath+L"..\\Bullet.ini");

	delete MyShipList;
	delete EnemyList;
	delete BulletList;
	for (size_t i=0; i<ShapeCount; i++) {
		delete ShapeList[i].Texture[0];
		delete ShapeList[i].Texture[1];
	}
	delete Font;
}

// ステージの設定
void CBulletGame::SetStage(int index) {
	StageIndex=(index+StageCount)%StageCount;
	DeleteTask(MyShipList);
	DeleteTask(EnemyList);
	DeleteTask(BulletList);
	
	STAGE* s=&Stage[StageIndex];
	new CMyShip(s->MyShipShapeId);
	s->Init();

	SlowUsed=false;
	TopTime=0;
}

// 動作
void CBulletGame::MoveTask(CTaskList* list) {
	for (CTaskIter i(list); i.HasNext(); ) {
		CMover* mover=static_cast<CMover*>(i.Next());
		mover->Move();
		if (!mover->Alive) i.Remove();
	}
}

void CBulletGame::Move() {

	// 入力
	Input->UpdateState();
	const CInputState* is=Input->GetState(0);
	InputState.Up=is->Up;
	InputState.Down=is->Down;
	InputState.Left=is->Left;
	InputState.Right=is->Right;
	InputState.Button[0]=is->Button[0];
	InputState.Button[1]=is->Button[1];

	if (!PrevInput) {
		if (is->Button[3]) Paused=!Paused;
		if (is->Button[4]) ShowHit=!ShowHit;
		if (is->Button[5]) ShowColor=!ShowColor;
		if (is->Button[6]) {
			Slow*=2;
			if (Slow>MaxSlow) Slow=1;
		}
	}

	// ステージの動作
	if (!Paused) {
		if (StageActive) {
			if (Slow>1) SlowUsed=true;
			if (!SlowUsed && TopTime>Stage[StageIndex].TopTime) {
				Stage[StageIndex].TopTime=TopTime;
			}
			if (MyShip->Alpha==1) {
				TopTime++;
			}
			
			if (Time==0) {
				MoveTask(MyShipList);
				MoveTask(BulletList);
				MoveTask(EnemyList);
			}
			Time=(Time+1)%Slow;
			
			if (!PrevInput && is->Button[2]) StageActive=false;
		} else {
			if (!PrevInput) {
				if (is->Button[2]) {
					StageActive=true;
					SetStage(StageIndex);
				} else
				if (is->Left) SetStage(StageIndex-StageOrder); else
				if (is->Right) SetStage(StageIndex+StageOrder); else
				if (is->Up) SetStage(StageIndex-10*StageOrder); else
				if (is->Down) SetStage(StageIndex+10*StageOrder);
			}
		}
	}
	
	// 前回の入力
	PrevInput=false;
	for (int i=2; i<32; i++) PrevInput=PrevInput||is->Button[i];
	if (!StageActive) PrevInput=PrevInput||is->Left||is->Right||is->Up||is->Down;
}

// 描画
void CBulletGame::DrawTask(CTaskList* list) {
	for (CTaskIter i(list); i.HasNext(); ) {
		CMover* mover=static_cast<CMover*>(i.Next());
		mover->Draw();
	}
}

void CBulletGame::Draw() {
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();
	Graphics->Clear(ShowColor?Black:White);

	DrawTask(MyShipList);
	DrawTask(BulletList);
	DrawTask(EnemyList);

	float 
		x=Graphics->GetHeight()*MaxX/MaxY, 
		y=0,
		dx=Graphics->GetWidth()-x,
		dy=Graphics->GetHeight();
	CTexture::DrawRect(device, x, y, dx, dy, ShowColor?MGray:Black);

	if (!StageActive) {
		Font->DrawText(wstring(L"\"")+Stage[StageIndex].Name+L"\"", 0, 0, ShowColor?White:Black);
		Font->DrawText(L"PUSH BUTTON2 OR [C] TO START", 0, 32, ShowColor?White:Black);
	}

	D3DCOLOR color=ShowColor?Black:White;
	Font->DrawText(L"STAGE", x, y, color);
	Font->DrawText(ToStr(StageIndex), x, y+32, color);
	y+=80;
	Font->DrawText(L"TIME", x, y, color);
	Font->DrawText(ToStr(TopTime), x, y+32, color);
	y+=80;
	Font->DrawText(L"TOP TIME", x, y, color);
	Font->DrawText(ToStr(Stage[StageIndex].TopTime), x, y+32, color);
	y+=80;
	Font->DrawText(L"PAUSE(B3,V)", x, y, color);
	Font->DrawText(Paused?L"ON":L"OFF", x, y+32, color);
	y+=80;
	Font->DrawText(L"HIT(B4,B)", x, y, color);
	Font->DrawText(ShowHit?L"ON":L"OFF", x, y+32, color);
	y+=80;
	Font->DrawText(L"COLOR(B5,N)", x, y, color);
	Font->DrawText(ShowColor?L"ON":L"OFF", x, y+32, color);
	y+=80;
	Font->DrawText(L"SLOW(B6,M)", x, y, color);
	Font->DrawText(ToStr(Slow), x, y+32, color);
}

// タスクの削除
void CBulletGame::DeleteTask(CTaskList* list) {
	for (CTaskIter i(list); i.HasNext(); i.Next(), i.Remove()) ;
}

// デバイスの復帰
void CBulletGame::OnResetDevice() {
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();

	// アルファブレンディングの設定
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// レンダリングの設定
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_ZENABLE, FALSE);
}
	
//==============================================================
// メインルーチン
INT WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, INT) {
	CBulletGame* game=new CBulletGame();
	game->SetStage((StageOrder-1)/2);
	game->Run();
	delete game;
	return 0;
}

