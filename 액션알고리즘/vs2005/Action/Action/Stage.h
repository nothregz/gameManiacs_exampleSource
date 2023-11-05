#ifndef STAGEH
#define STAGEH

#include "..\..\LibGame\LibGame\Task.h"
#include "Mover.h"

// 스테이지의 기본 클래스
class CStage {
public:
	const char* Name;
	int Score, Time;
	CStage(const char* name) : Name(name) {}
	virtual void Init() {}
	virtual bool Move(const CInputState* is) { return true; }
	virtual void Draw() {}
};

// 전체 스테이지 작성
extern void MakeStage();

#endif

