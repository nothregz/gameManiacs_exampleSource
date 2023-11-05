#pragma warning(disable:4786)
#pragma warning(disable:4819)
#pragma warning(disable:4311)

#include "Game.h"
#include "LibUtil\Archive.h"
#include <map>
#include <strstream>
using namespace std;


// 파일명에서 경로 찾아 반환
static string ExtractFilePath(string s) {
	int pos=s.rfind("\\");
	if (pos==string::npos) return "";
	return s.substr(0, pos+1);
}

// 실행파일 경로를 반환 (끝에 \ 붙음)
static string GetExePath() {
	char buf[MAX_PATH+1];
	GetModuleFileName(NULL, buf, MAX_PATH);
	return ExtractFilePath(buf);
}


//==============================================================
// HWND와 CGame 인스턴스를 연관짓는 맵
typedef map<HWND, CGame*, less<HWND> > THWndCGameMap;
static THWndCGameMap HWndCGameMap;


//==============================================================
// 빈 메시지 핸들러:
// 그래픽 등의 초기화 이전에는 이것을 실행한다.
static LRESULT WINAPI DummyWndProc(
	HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
) {
	return DefWindowProc( hwnd, msg, wparam, lparam );
}


//==============================================================
// 메시지 핸들러
static LRESULT WINAPI WndProc(
	HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
) {
	THWndCGameMap::iterator iter=HWndCGameMap.find(hwnd);
	if (iter==HWndCGameMap.end())
		return DefWindowProc( hwnd, msg, wparam, lparam );
	CGame* game=iter->second;
	return game->WndProc(msg, wparam, lparam);
}

LRESULT WINAPI CGame::WndProc(
	UINT msg, WPARAM wparam, LPARAM lparam
) {
	ostrstream oss;
	ostrstream oss_title;
	switch( msg )
	{

		// 그리기
		case WM_PAINT:
			DrawScene();
			break;

		// 마우스
		case WM_MOUSEMOVE:
			Mouse.X=LOWORD(lparam);
			Mouse.Y=HIWORD(lparam);
			break;
		case WM_LBUTTONDOWN: Mouse.LButton=true; break;
		case WM_LBUTTONUP: Mouse.LButton=false; break;
		case WM_MBUTTONDOWN: Mouse.MButton=true; break;
		case WM_MBUTTONUP: Mouse.MButton=false; break;
		case WM_RBUTTONDOWN: Mouse.RButton=true; break;
		case WM_RBUTTONUP: Mouse.RButton=false; break;

		// 제목 표시줄 더블클릭
		case WM_NCLBUTTONDBLCLK:
			ToggleFullScreen();
			return 0;

		// 종료
		case WM_CLOSE:
			if (!ConfirmExit ||
				MessageBox(HWnd, "종료하시겠습니까?", "종료확인", MB_OKCANCEL)==IDOK) {
				DestroyWindow(HWnd);
			}
			return 0;

		// 각종 명령
		case WM_COMMAND:
			switch (LOWORD(wparam)) {

				// 종료
				case IDC_EXIT:
					SendMessage(HWnd, WM_CLOSE, 0, 0);
					return 0;

				// Alt+Return 화면 모드 전환
				case IDC_TOGGLEFULLSCREEN:
					ToggleFullScreen();
					return 0;

				// 도움말
				case IDC_HELPURL:
					if (HelpURL!="") {
						ShellExecute(HWnd, NULL, HelpURL.c_str(),
    						NULL, NULL, SW_SHOWNORMAL);					
						return 0;
					}

				// 버전 표시
				case IDC_VERSION:
					oss<<"「"<<AppName<<"」\n\n";
					oss<<L"上記ソフトウェアの著作権は松浦健一郎／司ゆき（ひぐぺん工房）が所有しています。\n";
					oss<<"Copyright(C) 2004 Ken-ichiro Matsuura & Yuki Tsukasa (HigPen Works).\n";
					oss<<"All Rights Reserved.\n\n";
					oss<<L"[OK]を押すと下記のサポートWebサイトが開きます。\n";
					oss<<"（http://cgi32.plala.or.jp/higpen/gate.shtml）";
					oss<<ends;
					oss_title << L"バージョン情報";
					oss_title << ends;
					if( MessageBox( HWnd, oss.str( ), oss_title.str(), MB_OKCANCEL ) == IDOK )
					{
						ShellExecute(HWnd, NULL, 
							"http://cgi32.plala.or.jp/higpen/gate.shtml", 
    						NULL, NULL, SW_SHOWNORMAL);
					}
					return 0;
			}
			break;

		// 시스템 명령
		case WM_SYSCOMMAND:
			switch (wparam) {

				// 최대화 시에는 화면 모드 전환
				case SC_MAXIMIZE:
					ToggleFullScreen();
					return 0;					

				// 스크린 세이버와 모니터 전원 OFF 방지
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					return 0;

				// 메뉴
				case SC_KEYMENU:
					Graphics->PresentGDISurface();
					DrawMenuBar(HWnd);
					return DefWindowProc(HWnd, msg, wparam, lparam);
			}
			break;

		// 종료
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}
	return DefWindowProc(HWnd, msg, wparam, lparam);
}


//==============================================================
// <CGame> 생성자
BOOL FileExists( LPCTSTR szPath )
{
	DWORD dwAttrib = GetFileAttributes( szPath );

	return dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}


CGame::CGame( const char* app_name, bool zbuffer, bool use_menu, bool fixed_size )
:	Pause(false), MenuPause(false), UseMenu(use_menu), ConfirmExit(true),
	FPS(60), DropFrames(true), PauseInTheBackground(true),
	WindowWidth(800), WindowHeight(600), 
	FullScreenWidth(800), FullScreenHeight(600), 
	FullScreen(false), RefreshRate(60)
{
	// 애플리케이션 이름, 실행파일 경로, 도움말 URL
	AppName=app_name;
	ExePath=GetExePath();
	HelpURL="";

	// 아이콘 로드
	HINSTANCE hinst=GetModuleHandle(NULL);
	HICON icon;
	string icon_file=ExePath+"game.ico";
	if (FileExists(icon_file.c_str())) {
		icon=(HICON)LoadImage(
			hinst, icon_file.c_str(), 
			IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	} else {
		icon=LoadIcon(hinst, MAKEINTRESOURCE(1));
	}

	// 윈도우 클래스 등록:
	// 여기에서는 더미 메시지 핸들러를 등록한다.
	WNDCLASSEX wc={
		sizeof(WNDCLASSEX), CS_CLASSDC, DummyWndProc, 0L, 0L,
		hinst, icon, LoadCursor(NULL, IDC_ARROW), NULL, NULL,
		app_name, NULL
	};
	RegisterClassEx(&wc);

	// 액셀러레이터 키 설정:
	// [ESC] : 종료
	// [Alt]+[Enter] : 화면 모드 전환
	// [F1] : 도움말
	// [Shift]+[F1]：버전 표시
	ACCEL accel[]={
		{FVIRTKEY, VK_ESCAPE, IDC_EXIT},
		{FALT|FVIRTKEY, VK_RETURN, IDC_TOGGLEFULLSCREEN},
		{FVIRTKEY, VK_F1, IDC_HELPURL},
		{FSHIFT|FVIRTKEY, VK_F1, IDC_VERSION}
	};
	HAccel=CreateAcceleratorTable(
		accel, sizeof(accel)/sizeof(ACCEL));

	// 명령 라인 옵션을 해석
	for (int i=1; i<__argc; i++) {
		char* s=__argv[i];
		if (strcmp("-w", s)==0 && ++i<__argc) {
			WindowWidth=FullScreenWidth=atoi(__argv[i]);
		} else
		if (strcmp("-h", s)==0 && ++i<__argc) {
			WindowHeight=FullScreenHeight=atoi(__argv[i]);
		} else
		if (strcmp("-ww", s)==0 && ++i<__argc) {
			WindowWidth=atoi(__argv[i]);
		} else
		if (strcmp("-wh", s)==0 && ++i<__argc) {
			WindowHeight=atoi(__argv[i]);
		} else
		if (strcmp("-fw", s)==0 && ++i<__argc) {
			FullScreenWidth=atoi(__argv[i]);
		} else
		if (strcmp("-fh", s)==0 && ++i<__argc) {
			FullScreenHeight=atoi(__argv[i]);
		}
		if (strcmp("-r", s)==0 && ++i<__argc) {
			RefreshRate=atoi(__argv[i]);
		}
		if (strcmp("-f", s)==0) {
			FullScreen=true;
		}
		if (strcmp("-nce", s)==0) {
			ConfirmExit=false;
		}
	}

	// 윈도우 작성
	long style=WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
	RECT r={0, 0, WindowWidth, WindowHeight};
	AdjustWindowRect(&r, style, false);
	HWnd=CreateWindow(
		app_name, app_name, style, 
		0, 0, r.right-r.left, r.bottom-r.top, 
		GetDesktopWindow(), NULL, wc.hInstance, NULL);
	HWndCGameMap.insert(
		THWndCGameMap::value_type(HWnd, this));

	// 그래픽 작성
	Graphics=new CGraphics(HWnd);
	if (!Graphics) {
		string s=
			"이 프로그램은 DirectX 9.0 이상이 필요합니다.\n"
			"DirectX의 최신버전 및 최신 드라이버가\n"
			"설치되어 있는지 여부를 확인하십시오.\n";
		if (!fixed_size) {
			s+=
				"또한, 기동 시 옵션에서 해상도를 변경해 보세요.\n\n"
				"윈도우 800x600, 풀스크린 1024x768의 경우:\n"
				"-w 800 -h 600 -fw 1024 -fh 768";
		}
		MessageBox(HWnd, s.c_str(), app_name, MB_OK);
		exit(1);
	}
	Graphics->Clear();
	Graphics->Present();
	if (FullScreen) ResetScreen();

	// 입력의 초기화
	Input=new CInput(HWnd);
	Mouse.X=Mouse.Y=0;
	Mouse.LButton=Mouse.MButton=Mouse.RButton=false;

	// 시간
	LastPerfCounter.QuadPart=0;
	LastTickCount=0;
	Elapsed=0;

	// 메뉴
	if (UseMenu) {
		MenuBar=CreateMenu();
		SetMenu(HWnd, MenuBar);
	}
}


//==============================================================
static const char* copyright=
	"Copyright(C) 2005 마츠우라 켄이치로/ 츠카사 유키";


//==============================================================
// 메뉴
HMENU CGame::AddMenu(string text) {
	HMENU menu=CreatePopupMenu();
	AppendMenu(MenuBar, MF_ENABLED | MF_STRING | MF_POPUP, (UINT)menu, text.c_str());
	DrawMenuBar(HWnd);
	return menu;
}
HMENU CGame::AddSubMenu(HMENU menu, string text) {
	HMENU sub_menu=CreatePopupMenu();
	AppendMenu(menu, MF_ENABLED | MF_STRING | MF_POPUP, (UINT)sub_menu, text.c_str());
	return sub_menu;
}
void CGame::AddMenuItem(HMENU menu, string text, int id) {
	AppendMenu(menu, MF_ENABLED | MF_STRING, id, text.c_str());
}
void CGame::AddMenuSeparator(HMENU menu) {
	AppendMenu(menu, MF_ENABLED | MF_SEPARATOR, 0, NULL);
}
void CGame::CheckMenuItem(HMENU menu, int id, bool check) {
	MENUITEMINFO mii;
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask=MIIM_STATE;
	mii.fState=check?MFS_CHECKED:MFS_UNCHECKED;
	SetMenuItemInfo(menu, id, FALSE, &mii);
}


//==============================================================
// ＜CGame＞소멸자
CGame::~CGame() {
	delete Input;
	delete Graphics;
}


//==============================================================
// ＜CGame＞시각 재설정
void CGame::ResetTime() {
	LARGE_INTEGER freq, count;
	if (QueryPerformanceFrequency(&freq) && QueryPerformanceCounter(&count)) {
		LastPerfCounter=count;
	} else {
		LastTickCount=GetTickCount();
	}
}

// 화면 재설정
void CGame::ResetScreen() {
	if (FullScreen) {
		Graphics->SetWidth(FullScreenWidth);
		Graphics->SetHeight(FullScreenHeight);
		Graphics->SetRefreshRate(RefreshRate);
	} else {
		Graphics->SetWidth(WindowWidth);
		Graphics->SetHeight(WindowHeight);
		Graphics->SetRefreshRate(0);
	}
	Graphics->SetFullScreen(FullScreen);
	OnLostDevice();
	if (Graphics->ResetDevice()) OnResetDevice();
}

// 윈도우 모드와 풀스크린 모드 전환
void CGame::ToggleFullScreen() {
	FullScreen=!Graphics->IsFullScreen();
	ResetScreen();
}

//==============================================================
// ＜CGame＞게임 실행
void CGame::Run() {

	// 디바이스 재설정 이후 처리를 호출
	OnResetDevice();

	// 더미 메시지 핸들러를,
	// 정식 메시지 핸들러로 대체
	SetWindowLong(HWnd, GWL_WNDPROC, (long)::WndProc);
	
	// 메시지 루프
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ResetTime();
	while (msg.message!=WM_QUIT) {

		// 백그라운드 시 메시지 처리
		if (PauseInTheBackground && HWnd!=GetForegroundWindow()) {
			GetMessage(&msg, NULL, 0U, 0U);
			if (!TranslateAccelerator(HWnd, HAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			ResetTime();
			continue;
		}
		
		// 포어그라운드(foreground)시 메시지 처리
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			if (!TranslateAccelerator(HWnd, HAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			continue;
		}

		// 풀스크린 시 메뉴 표시
		if (UseMenu) {
			if (Graphics->IsFullScreen()) {
				POINT p;
				GetCursorPos(&p);
				bool prev=MenuPause;
				MenuPause=(p.y<GetSystemMetrics(SM_CYMENU));
				if (!prev && MenuPause) {
					Graphics->PresentGDISurface();
					DrawMenuBar(HWnd);
				}
			} else {
				MenuPause=false;
			}
		}

		// 일시정지
		if (Pause || MenuPause) continue;

		// 시간조정
		LARGE_INTEGER freq, count;
		DWORD tick;
		if (QueryPerformanceFrequency(&freq) && 
			QueryPerformanceCounter(&count)) {
			Elapsed+=FPS*(count.QuadPart-LastPerfCounter.QuadPart)/freq.QuadPart;
			LastPerfCounter=count;
		} else {
			tick=GetTickCount();
			Elapsed+=FPS*(tick-LastTickCount)/1000;
			LastTickCount=tick;
		}

		// 이동 및 그리기
		if (Graphics->GetRefreshRate()==FPS || Elapsed>=1.0) {
			if (DropFrames) {
				for (; Elapsed>=1.0; Elapsed-=1.0) Move();
			} else {
				Elapsed=0;
				Move();
			}
			DrawScene();
		}
	}

	// 메시지 핸들러를 더미로 되돌리기
	SetWindowLong(HWnd, GWL_WNDPROC, (long)::DummyWndProc);
	
	// 종료처리
	UnregisterClass(AppName.c_str(), GetModuleHandle(NULL));
}

// 그리기
void CGame::DrawScene() {

	// 디바이스가 없는 경우 재구축
	switch (Graphics->GetDevice()->TestCooperativeLevel()) {
		case D3DERR_DEVICELOST:
			Sleep(50);
			return;
		case D3DERR_DEVICENOTRESET:
			OnLostDevice();
			if (Graphics->ResetDevice()) OnResetDevice();
			return;
	}

	// 그리기 시작
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();
	Graphics->BeginScene();

	Draw();

	// 그리기 완료
	Graphics->EndScene();
	Graphics->Present();
}

