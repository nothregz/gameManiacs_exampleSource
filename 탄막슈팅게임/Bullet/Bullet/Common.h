#ifndef COMMONH
#define COMMONH

#pragma warning(disable:4244 4786 4819 4996)
#include "..\..\LibGame\LibGame.h"
class CMyShip;

//==============================================================
// シェイプ
struct SHAPE {
	const wchar_t* TextureName;
	float Size;
	float Hit;
	CTexture* Texture[2];
};
extern SHAPE ShapeList[];
extern size_t ShapeCount;

//==============================================================
// シェイプID
enum {
	ShVoid, ShHit, 
	DpMyShip, DpEnemy, DpRedBullet, DpBlueBullet, 
	PsMyShip, PsEnemy, PsNeedle, PsBullet,
	IkBlackMyShip, IkWhiteMyShip, IkEnemy, IkBlackBullet, IkWhiteBullet,
	ThMyShip, ThEnemy, ThBlackBullet, ThWhiteBullet,
};

//==============================================================
// ステージ
struct STAGE {
	const wchar_t* Name;
	void (*Init)();
	int MyShipShapeId;
	int TopTime;
};
extern STAGE Stage[];
extern size_t StageCount;

//==============================================================
// グローバル変数
extern CGraphics* Graphics;
extern CTaskList* MyShipList;
extern CTaskList* EnemyList;
extern CTaskList* BulletList;
extern CMyShip* MyShip;

extern CRand Rand;
extern CInputState InputState;

extern bool ShowHit;
extern bool ShowColor;

//==============================================================
// 定数
#define MaxX 1
#define MaxY 1

#define StageOrder 1
#define MyShipHighSpeed 0.014f
#define MyShipLowSpeed 0.007f
#define MaxSlow 8

#define White D3DCOLOR_ARGB(255, 255, 255, 255)
#define LGray D3DCOLOR_ARGB(255, 192, 12, 192)
#define MGray D3DCOLOR_ARGB(255, 128, 128, 128)
#define DGray D3DCOLOR_ARGB(255, 64, 64, 64)
#define Black D3DCOLOR_ARGB(255, 0, 0, 0)
#define Shade D3DCOLOR_ARGB(128, 0, 0, 0)

#endif
