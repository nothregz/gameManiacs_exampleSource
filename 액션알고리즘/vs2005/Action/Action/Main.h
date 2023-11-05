#ifndef MAINH
#define MAINH

#pragma warning(disable:4244 4786 4819 4996)

#include "..\..\LibGame\LibGame\LibGame.h"
#include "..\..\LibUtil\LibUtil\LibUtil.h"
#include "Stage.h"


// 텍스처
enum {
	TEX_COIN, TEX_ENEMY0, TEX_ENEMY1, TEX_FISH, 
	TEX_FLOOR, TEX_ICE_FLOOR, TEX_ITEM, TEX_LIFT, 
	TEX_MAN, TEX_SWIMMER, TEX_TREE, TEX_LOOP, 
	TEX_STAMP_BOARD, TEX_ROPE, TEX_LADDER, 
	TEX_TRAMPOLINE0, TEX_TRAMPOLINE1, TEX_TRAMPOLINE2, 
	TEX_DROP_FLOOR, TEX_REVOLVING_DOOR, 
	TEX_DOOR0, TEX_DOOR1, TEX_DOOR2, TEX_DOOR3, 
	TEX_ELEVATOR0, TEX_ELEVATOR1, 
	TEX_MOVING_FLOOR, TEX_CONVEYOR0, TEX_CONVEYOR1, 
	TEX_AIR_CURRENT0, TEX_AIR_CURRENT1, 
	TEX_WARP_GATE, TEX_ROCK, TEX_CLAY, TEX_OBJECT, 
	TEX_ICE, TEX_HOLE, TEX_FOOTHOLD, TEX_PANEL, 
	TEX_PILE, TEX_CRACK, 
	TEX_SKATEBOARD, TEX_CAR, TEX_ANIMAL, 
	TEX_SEESAW0, TEX_SEESAW1, TEX_CROUCH, 
	TEX_SWORD, TEX_PEPPER, TEX_BOMB, TEX_LIGHT, 
	TEX_SHURIKEN, TEX_FILL, TEX_BALL, TEX_FIRE, TEX_BLAST, 
	TEX_MISSILE, TEX_BULLET, TEX_BOOMERANG, TEX_SHIELD, 
	TEX_BUBBLE, TEX_MEDICINE, TEX_HEART, TEX_MUSHROOM, TEX_JEWEL, 
	TEX_COUNT
};


//==============================================================
// 게임 본체 클래스
class CActGame : public CGame {
protected:
	bool Paused, PrevInput;
public:

	// 생성자, 소멸자
	CActGame();
	~CActGame();
	
	// 디바이스 초기화, 이동, 그리기
	virtual void OnResetDevice();
	virtual void Move();
	virtual void Draw();

	// 폰트
	CFont* Font;

	// 텍스처
	vector<CTexture*> Texture;

	// 작업목록
	CTaskList *MoverList;
	void MoveTask(CTaskList* list);
	void DrawTask(CTaskList* list);

	// 스테이지
	vector<CStage*> Stage;
	int StageIndex;
	bool StageActive;
	void SetStage(int stage);

	// 난수
	CRand Rand;
};


//==============================================================
// 글로벌 변수, 정수

// 게임 본체
extern CActGame* Game;

// 색
#define COL_WHITE D3DCOLOR_ARGB(255, 255, 255, 255)
#define COL_LGRAY D3DCOLOR_ARGB(255, 192, 192, 192)
#define COL_MGRAY D3DCOLOR_ARGB(255, 128, 128, 128)
#define COL_DGRAY D3DCOLOR_ARGB(255, 64, 64, 64)
#define COL_BLACK D3DCOLOR_ARGB(255, 0, 0, 0)
#define COL_SHADE D3DCOLOR_ARGB(128, 0, 0, 0)

// 좌표
#define MAX_X 16
#define MAX_Y 12

#endif
