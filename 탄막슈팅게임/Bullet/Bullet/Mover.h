#ifndef MOVERH
#define MOVERH

#include "Common.h"

//==============================================================
// �ړ�����
class CMover : public CTask {
public:
	SHAPE* Shape;
	float X, Y, Angle;
	float Scale, Alpha;
	bool Alive;
	int Color;
	
	CMover(CTaskList* list, int shape_id, float x, float y, float angle);

	virtual void Move();

	void DrawSub(CTexture* texture, float size, float fade);
	void Draw();
	
	bool IsHit(CMover* mover);
	bool IsHit(CTaskList* list);
	
	void SetShape(int shape_id);
};

//==============================================================
// ���@
class CMyShip : public CMover {
public:
	bool PrevButton;
	void* operator new(size_t n);
	void operator delete(void* p);
	CMyShip(int shape_id);
	void Move();
};

class CMyShipCrash : public CMyShip {
public:
	CMyShipCrash(CMyShip* myship);
	void Move();
};

//==============================================================
// �G
class CEnemy : public CMover {
public:
	void* operator new(size_t n);
	void operator delete(void* p);
	CEnemy(int shape_id);
};

//==============================================================
// �e
class CBullet : public CMover {
public:
	float AngleRate;
	float Speed, SpeedRate;

	void* operator new(size_t n);
	void operator delete(void* p);
	CBullet(
		int shape_id, float x, float y, 
		float angle, float angle_rate, 
		float speed, float speed_rate);
	void Move();
};

//==============================================================
// �T�u���[�`��

float GetMyShipAngle(float x, float y);
float GetAngle(float x, float y, float to_x, float to_y);

void ShootNWay(
	int shape_id, float x, float y, float angle, float angle_range, 
	float speed, int count, float angle_rate, float speed_rate
);

#endif

