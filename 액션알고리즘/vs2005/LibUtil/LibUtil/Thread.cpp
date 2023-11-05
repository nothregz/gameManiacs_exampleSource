#include "Thread.h"


//==============================================================
// 스레드

// 스레드 기동 함수
static DWORD WINAPI ThreadProc(LPVOID thread) {
	return ((CThread*)thread)->Run();
}

// 생성자
CThread::CThread(DWORD stackSize) {
	Thread=CreateThread(NULL, stackSize, ThreadProc, (LPVOID)this, CREATE_SUSPENDED, &ID);
}

// 소멸자
CThread::~CThread() {
	CloseHandle(Thread);
}

