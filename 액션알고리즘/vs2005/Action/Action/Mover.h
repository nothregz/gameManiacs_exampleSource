#ifndef MOVERH
#define MOVERH

#include "..\..\LibGame\LibGame\Task.h"
#include "..\..\LibGame\LibGame\Texture.h"

//==============================================================
// 이동 물체
class CMover : public CTask {
public:
	float X, Y, W, H, Angle, ShearX;
	int Type, DrawOrder;
	CTexture* Texture;
	D3DCOLOR Color;
	bool ReverseX;

	// 생성자
	CMover();
	void* operator new(size_t t);
	void operator delete(void* p);
	
	// 이동, 그리기
	virtual bool Move(const CInputState* is) { return true; }
	virtual void Draw();
};

#endif

