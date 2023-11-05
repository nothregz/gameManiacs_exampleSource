#pragma warning(disable:4786)
#pragma warning(disable:4819)
#pragma warning(disable:4311)

#include "Game.h"
#include "..\LibUtil\LibUtil.h"
#include <map>
#include <sstream>
using namespace std;


// �t�@�C��������p�X�����o���ĕԂ�
static wstring ExtractFilePath(wstring s) {
	int pos=s.rfind(L"\\");
	if (pos==string::npos) return L"";
	return s.substr(0, pos+1);
}

// ���s�t�@�C���̃p�X��Ԃ��i������\���t���j
static wstring GetExePath() {
	wchar_t buf[MAX_PATH+1];
	GetModuleFileName(NULL, buf, MAX_PATH);
	return ExtractFilePath(buf);
}


//==============================================================
// HWND��CGame�C���X�^���X���֘A�Â���}�b�v
typedef map<HWND, CGame*, less<HWND> > THWndCGameMap;
static THWndCGameMap HWndCGameMap;


//==============================================================
// ��̃��b�Z�[�W�n���h���F
// �O���t�B�b�N�X�Ȃǂ̏������O�ɂ͂�������s����B
static LRESULT WINAPI DummyWndProc(
	HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
) {
	return DefWindowProc( hwnd, msg, wparam, lparam );
}


//==============================================================
// ���b�Z�[�W�n���h��
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
	wostringstream oss;
	switch(msg) {

		// �`��
		case WM_PAINT:
			DrawScene();
			break;

		// �}�E�X
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

		// �^�C�g���o�[�̃_�u���N���b�N
		case WM_NCLBUTTONDBLCLK:
			ToggleFullScreen();
			return 0;

		// �I��
		case WM_CLOSE:
			if (!ConfirmExit ||
				MessageBox(HWnd, L"�I�����Ă���낵���ł����H", L"�I���m�F", MB_OKCANCEL)==IDOK) {
				DestroyWindow(HWnd);
			}
			return 0;

		// �e��̃R�}���h
		case WM_COMMAND:
			switch (LOWORD(wparam)) {

				// �I��
				case IDC_EXIT:
					SendMessage(HWnd, WM_CLOSE, 0, 0);
					return 0;

				// Alt+Return�ŉ�ʃ��[�h�؂�ւ�
				case IDC_TOGGLEFULLSCREEN:
					ToggleFullScreen();
					return 0;

				// �w���v
				case IDC_HELPURL:
					if (HelpURL!=L"") {
						ShellExecute(HWnd, NULL, HelpURL.c_str(),
    						NULL, NULL, SW_SHOWNORMAL);					
						return 0;
					}

				// �o�[�W�����\��
				case IDC_VERSION:
					oss<<L"�u"<<AppName<<L"�v\n\n";
					oss<<L"��L�\�t�g�E�F�A�̒��쌠�͏��Y����Y�^�i�䂫�i�Ђ��؂�H�[�j�����L���Ă��܂��B\n";
					oss<<L"Copyright(C) 2004 Ken-ichiro Matsuura & Yuki Tsukasa (HigPen Works).\n";
					oss<<L"All Rights Reserved.\n\n";
					oss<<L"[OK]�������Ɖ��L�̃T�|�[�gWeb�T�C�g���J���܂��B\n";
					oss<<L"�ihttp://cgi32.plala.or.jp/higpen/gate.shtml�j";
					oss<<ends;
					if (MessageBox(HWnd, oss.str().c_str(), L"�o�[�W�������", MB_OKCANCEL)==IDOK) {
						ShellExecute(HWnd, NULL, 
							L"http://cgi32.plala.or.jp/higpen/gate.shtml", 
    						NULL, NULL, SW_SHOWNORMAL);
					}
					return 0;
			}
			break;

		// �V�X�e���R�}���h
		case WM_SYSCOMMAND:
			switch (wparam) {

				// �ő剻���ɂ͉�ʃ��[�h�؂�ւ�
				case SC_MAXIMIZE:
					ToggleFullScreen();
					return 0;					

				// �X�N���[���Z�[�o�ƃ��j�^�d��Off�̖h�~
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					return 0;

				// ���j���[
				case SC_KEYMENU:
					Graphics->PresentGDISurface();
					DrawMenuBar(HWnd);
					return DefWindowProc(HWnd, msg, wparam, lparam);
			}
			break;

		// �I��
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}
	return DefWindowProc(HWnd, msg, wparam, lparam);
}


//==============================================================
// ��CGame���R���X�g���N�^
CGame::CGame(const wchar_t* app_name, bool zbuffer, bool use_menu, bool fixed_size)
:	Pause(false), MenuPause(false), UseMenu(use_menu), ConfirmExit(true),
	FPS(60), DropFrames(true), PauseInTheBackground(true),
	WindowWidth(800), WindowHeight(600), 
	FullScreenWidth(800), FullScreenHeight(600), 
	FullScreen(false), RefreshRate(60)
{
	// �A�v���P�[�V�������C���s�t�@�C���̃p�X�A�w���v�̂t�q�k
	AppName=app_name;
	ExePath=GetExePath();
	HelpURL=L"";

	// �A�C�R���̓ǂݍ���
	HINSTANCE hinst=GetModuleHandle(NULL);
	HICON icon;
	wstring icon_file=ExePath+L"game.ico";
	if (FileExists(icon_file)) {
		icon=(HICON)LoadImage(
			hinst, icon_file.c_str(), 
			IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	} else {
		icon=LoadIcon(hinst, MAKEINTRESOURCE(1));
	}

	// �E�B���h�E�N���X�̓o�^�F
	// �����ł̓_�~�[�̃��b�Z�[�W�n���h����o�^����
	WNDCLASSEX wc={
		sizeof(WNDCLASSEX), CS_CLASSDC, DummyWndProc, 0L, 0L,
		hinst, icon, LoadCursor(NULL, IDC_ARROW), NULL, NULL,
		app_name, NULL
	};
	RegisterClassEx(&wc);

	// �A�N�Z�����[�^�L�[�̐ݒ�F
	// [ESC] : �I��
	// [Alt]+[Enter] : ��ʃ��[�h�؂�ւ�
	// [F1] : �w���v
	// [Shift]+[F1]�F�o�[�W�����\��
	ACCEL accel[]={
		{FVIRTKEY, VK_ESCAPE, IDC_EXIT},
		{FALT|FVIRTKEY, VK_RETURN, IDC_TOGGLEFULLSCREEN},
		{FVIRTKEY, VK_F1, IDC_HELPURL},
		{FSHIFT|FVIRTKEY, VK_F1, IDC_VERSION}
	};
	HAccel=CreateAcceleratorTable(
		accel, sizeof(accel)/sizeof(ACCEL));

	// �R�}���h���C���I�v�V�����̉���
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

	// �E�B���h�E�̍쐬
	long style=WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
	RECT r={0, 0, WindowWidth, WindowHeight};
	AdjustWindowRect(&r, style, false);
	HWnd=CreateWindow(
		app_name, app_name, style, 
		0, 0, r.right-r.left, r.bottom-r.top, 
		GetDesktopWindow(), NULL, wc.hInstance, NULL);
	HWndCGameMap.insert(
		THWndCGameMap::value_type(HWnd, this));

	// �O���t�B�b�N�X�̍쐬
	Graphics=new CGraphics(HWnd);
	if (!Graphics) {
		wstring s=
			L"���̃v���O�����ɂ�DirectX 9.0�ȏオ�K�v�ł��B\n"
			L"DirectX�̍ŐV�ŁA����эŐV�̃h���C�o��\n"
			L"�C���X�g�[������Ă��邩�ǂ������m�F���������B\n";
		if (!fixed_size) {
			s+=
				L"�܂��A�N�����̃I�v�V�����ŉ𑜓x��ύX���Ă݂Ă��������B\n\n"
				L"�E�B���h�E��800x600�A�t���X�N���[��1024x768�̏ꍇ�F\n"
				L"-w 800 -h 600 -fw 1024 -fh 768";
		}
		MessageBox(HWnd, s.c_str(), app_name, MB_OK);
		exit(1);
	}
	Graphics->Clear();
	Graphics->Present();
	if (FullScreen) ResetScreen();

	// ���͂̏�����
	Input=new CInput(HWnd);
	Mouse.X=Mouse.Y=0;
	Mouse.LButton=Mouse.MButton=Mouse.RButton=false;

	// ����
	LastPerfCounter.QuadPart=0;
	LastTickCount=0;
	Elapsed=0;

	// ���j���[
	if (UseMenu) {
		MenuBar=CreateMenu();
		SetMenu(HWnd, MenuBar);
	}
}


//==============================================================
static const char* copyright=
	"Copyright(C) 2005 �Ђ��؂�H�[�i���Y����Y�^�i�䂫�j";


//==============================================================
// ���j���[
HMENU CGame::AddMenu(wstring text) {
	HMENU menu=CreatePopupMenu();
	AppendMenu(MenuBar, MF_ENABLED | MF_STRING | MF_POPUP, (UINT)menu, text.c_str());
	DrawMenuBar(HWnd);
	return menu;
}
HMENU CGame::AddSubMenu(HMENU menu, wstring text) {
	HMENU sub_menu=CreatePopupMenu();
	AppendMenu(menu, MF_ENABLED | MF_STRING | MF_POPUP, (UINT)sub_menu, text.c_str());
	return sub_menu;
}
void CGame::AddMenuItem(HMENU menu, wstring text, int id) {
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
// ��CGame���f�X�g���N�^
CGame::~CGame() {
	delete Input;
	delete Graphics;
}


//==============================================================
// ��CGame�������̃��Z�b�g
void CGame::ResetTime() {
	LARGE_INTEGER freq, count;
	if (QueryPerformanceFrequency(&freq) && QueryPerformanceCounter(&count)) {
		LastPerfCounter=count;
	} else {
		LastTickCount=GetTickCount();
	}
}

// ��ʂ̃��Z�b�g
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

// �E�B���h�E���[�h�ƃt���X�N���[�����[�h�̐؂�ւ�
void CGame::ToggleFullScreen() {
	FullScreen=!Graphics->IsFullScreen();
	ResetScreen();
}

//==============================================================
// ��CGame���Q�[���̎��s
void CGame::Run() {

	// �f�o�C�X���Z�b�g��̏������Ăяo��
	OnResetDevice();

	// �_�~�[�̃��b�Z�[�W�n���h�����C
	// �����ȃ��b�Z�[�W�n���h���ɒu��������
	SetWindowLong(HWnd, GWL_WNDPROC, (long)::WndProc);
	
	// ���b�Z�[�W���[�v
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ResetTime();
	while (msg.message!=WM_QUIT) {

		// �o�b�N�O���E���h���̃��b�Z�[�W����
		if (PauseInTheBackground && HWnd!=GetForegroundWindow()) {
			GetMessage(&msg, NULL, 0U, 0U);
			if (!TranslateAccelerator(HWnd, HAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			ResetTime();
			continue;
		}
		
		// �t�H�A�O���E���h���̃��b�Z�[�W����
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			if (!TranslateAccelerator(HWnd, HAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			continue;
		}

		// �t���X�N���[�����̃��j���[�\��
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

		// �ꎞ��~
		if (Pause || MenuPause) continue;

		// ���Ԓ���
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

		// �ړ��ƕ`��
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

	// ���b�Z�[�W�n���h�����_�~�[�ɖ߂�
	SetWindowLong(HWnd, GWL_WNDPROC, (long)::DummyWndProc);
	
	// �I������
	UnregisterClass(AppName.c_str(), GetModuleHandle(NULL));
}

// �`��
void CGame::DrawScene() {

	// �f�o�C�X���������Ă�����č\�z����
	switch (Graphics->GetDevice()->TestCooperativeLevel()) {
		case D3DERR_DEVICELOST:
			Sleep(50);
			return;
		case D3DERR_DEVICENOTRESET:
			OnLostDevice();
			if (Graphics->ResetDevice()) OnResetDevice();
			return;
	}

	// �`��J�n
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();
	Graphics->BeginScene();

	Draw();

	// �`�抮��
	Graphics->EndScene();
	Graphics->Present();
}

