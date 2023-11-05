#ifndef GAMEH
#define GAMEH

#include "Graphics.h"
#include "Input.h"
#include <vector>
#include <string>
using namespace std;


#define IDC_EXIT 0
#define IDC_TOGGLEFULLSCREEN 1
#define IDC_HELPURL 2
#define IDC_VERSION 3


// 게임 본체 클래스
class CGame {
protected:

	// 액셀러레이터 키
	HACCEL HAccel;

	// 윈도우 핸들
	HWND HWnd;

	// 그래픽
	CGraphics* Graphics;

	// 입력
	CInput* Input;
	MOUSE_STATE Mouse;

	// 실행 파일의 경로, 애플리케이션 이름, 도움말 URL
	string ExePath, AppName, HelpURL;

	// 속도 조정
	LARGE_INTEGER LastPerfCounter;
	DWORD LastTickCount;
	double Elapsed;
	void ResetTime();

	// 일시정지
	bool Pause, MenuPause;

	// 메뉴
	bool UseMenu;
	HMENU MenuBar;

	// 종료확인
	bool ConfirmExit;

	// 화면 크기, 재생률
	int WindowWidth, WindowHeight, FullScreenWidth, FullScreenHeight,
		RefreshRate;
	bool FullScreen;
	
	void DrawScene();

public:

	// 생성자, 소멸자
	CGame(const char* app_name, bool zbuffer, bool use_menu, bool fixed_size);
	~CGame();

	// 메시지 핸들러
	virtual LRESULT WINAPI WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

	// 게임 실행
	void Run();

	// 각종 설정
	double FPS;
	bool DropFrames, PauseInTheBackground;

	// 이동 및 그리기:
	// 하위 클래스에서 오버라이드 
	virtual void Move() {}
	virtual void Draw() {}
	
	// Direct3D 리소스 관리
	virtual void OnLostDevice() {}
	virtual void OnResetDevice() {}

	// 메뉴
	HMENU GetMenuBar() { return MenuBar; }
	HMENU AddMenu(string text);
	HMENU AddSubMenu(HMENU menu, string text);
	void AddMenuItem(HMENU menu, string text, int id);
	void AddMenuSeparator(HMENU menu);
	void CheckMenuItem(HMENU menu, int id, bool check);

	// 각종 정보 취득
	CGraphics* GetGraphics() { return Graphics; }
	CInput* GetInput() { return Input; }
	const MOUSE_STATE& GetMouseState() { return Mouse; }

	// 종료확인
	void SetConfirmExit(bool ce) { ConfirmExit=ce; }

	// 화면 모드로 전환
	void ResetScreen();
	void ToggleFullScreen();
};


#endif
