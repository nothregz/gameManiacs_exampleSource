#pragma warning(disable:4786)
#pragma warning(disable:4819)
#pragma warning(disable:4311)

#include "Game.h"
#include "..\LibUtil\LibUtil.h"
#include <map>
#include <sstream>
using namespace std;


// ファイル名からパスを取り出して返す
static wstring ExtractFilePath(wstring s) {
	int pos=s.rfind(L"\\");
	if (pos==string::npos) return L"";
	return s.substr(0, pos+1);
}

// 実行ファイルのパスを返す（末尾に\が付く）
static wstring GetExePath() {
	wchar_t buf[MAX_PATH+1];
	GetModuleFileName(NULL, buf, MAX_PATH);
	return ExtractFilePath(buf);
}


//==============================================================
// HWNDとCGameインスタンスを関連づけるマップ
typedef map<HWND, CGame*, less<HWND> > THWndCGameMap;
static THWndCGameMap HWndCGameMap;


//==============================================================
// 空のメッセージハンドラ：
// グラフィックスなどの初期化前にはこれを実行する。
static LRESULT WINAPI DummyWndProc(
	HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
) {
	return DefWindowProc( hwnd, msg, wparam, lparam );
}


//==============================================================
// メッセージハンドラ
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

		// 描画
		case WM_PAINT:
			DrawScene();
			break;

		// マウス
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

		// タイトルバーのダブルクリック
		case WM_NCLBUTTONDBLCLK:
			ToggleFullScreen();
			return 0;

		// 終了
		case WM_CLOSE:
			if (!ConfirmExit ||
				MessageBox(HWnd, L"終了してもよろしいですか？", L"終了確認", MB_OKCANCEL)==IDOK) {
				DestroyWindow(HWnd);
			}
			return 0;

		// 各種のコマンド
		case WM_COMMAND:
			switch (LOWORD(wparam)) {

				// 終了
				case IDC_EXIT:
					SendMessage(HWnd, WM_CLOSE, 0, 0);
					return 0;

				// Alt+Returnで画面モード切り替え
				case IDC_TOGGLEFULLSCREEN:
					ToggleFullScreen();
					return 0;

				// ヘルプ
				case IDC_HELPURL:
					if (HelpURL!=L"") {
						ShellExecute(HWnd, NULL, HelpURL.c_str(),
    						NULL, NULL, SW_SHOWNORMAL);					
						return 0;
					}

				// バージョン表示
				case IDC_VERSION:
					oss<<L"「"<<AppName<<L"」\n\n";
					oss<<L"上記ソフトウェアの著作権は松浦健一郎／司ゆき（ひぐぺん工房）が所有しています。\n";
					oss<<L"Copyright(C) 2004 Ken-ichiro Matsuura & Yuki Tsukasa (HigPen Works).\n";
					oss<<L"All Rights Reserved.\n\n";
					oss<<L"[OK]を押すと下記のサポートWebサイトが開きます。\n";
					oss<<L"（http://cgi32.plala.or.jp/higpen/gate.shtml）";
					oss<<ends;
					if (MessageBox(HWnd, oss.str().c_str(), L"バージョン情報", MB_OKCANCEL)==IDOK) {
						ShellExecute(HWnd, NULL, 
							L"http://cgi32.plala.or.jp/higpen/gate.shtml", 
    						NULL, NULL, SW_SHOWNORMAL);
					}
					return 0;
			}
			break;

		// システムコマンド
		case WM_SYSCOMMAND:
			switch (wparam) {

				// 最大化時には画面モード切り替え
				case SC_MAXIMIZE:
					ToggleFullScreen();
					return 0;					

				// スクリーンセーバとモニタ電源Offの防止
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					return 0;

				// メニュー
				case SC_KEYMENU:
					Graphics->PresentGDISurface();
					DrawMenuBar(HWnd);
					return DefWindowProc(HWnd, msg, wparam, lparam);
			}
			break;

		// 終了
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}
	return DefWindowProc(HWnd, msg, wparam, lparam);
}


//==============================================================
// ＜CGame＞コンストラクタ
CGame::CGame(const wchar_t* app_name, bool zbuffer, bool use_menu, bool fixed_size)
:	Pause(false), MenuPause(false), UseMenu(use_menu), ConfirmExit(true),
	FPS(60), DropFrames(true), PauseInTheBackground(true),
	WindowWidth(800), WindowHeight(600), 
	FullScreenWidth(800), FullScreenHeight(600), 
	FullScreen(false), RefreshRate(60)
{
	// アプリケーション名，実行ファイルのパス、ヘルプのＵＲＬ
	AppName=app_name;
	ExePath=GetExePath();
	HelpURL=L"";

	// アイコンの読み込み
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

	// ウィンドウクラスの登録：
	// ここではダミーのメッセージハンドラを登録する
	WNDCLASSEX wc={
		sizeof(WNDCLASSEX), CS_CLASSDC, DummyWndProc, 0L, 0L,
		hinst, icon, LoadCursor(NULL, IDC_ARROW), NULL, NULL,
		app_name, NULL
	};
	RegisterClassEx(&wc);

	// アクセラレータキーの設定：
	// [ESC] : 終了
	// [Alt]+[Enter] : 画面モード切り替え
	// [F1] : ヘルプ
	// [Shift]+[F1]：バージョン表示
	ACCEL accel[]={
		{FVIRTKEY, VK_ESCAPE, IDC_EXIT},
		{FALT|FVIRTKEY, VK_RETURN, IDC_TOGGLEFULLSCREEN},
		{FVIRTKEY, VK_F1, IDC_HELPURL},
		{FSHIFT|FVIRTKEY, VK_F1, IDC_VERSION}
	};
	HAccel=CreateAcceleratorTable(
		accel, sizeof(accel)/sizeof(ACCEL));

	// コマンドラインオプションの解釈
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

	// ウィンドウの作成
	long style=WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
	RECT r={0, 0, WindowWidth, WindowHeight};
	AdjustWindowRect(&r, style, false);
	HWnd=CreateWindow(
		app_name, app_name, style, 
		0, 0, r.right-r.left, r.bottom-r.top, 
		GetDesktopWindow(), NULL, wc.hInstance, NULL);
	HWndCGameMap.insert(
		THWndCGameMap::value_type(HWnd, this));

	// グラフィックスの作成
	Graphics=new CGraphics(HWnd);
	if (!Graphics) {
		wstring s=
			L"このプログラムにはDirectX 9.0以上が必要です。\n"
			L"DirectXの最新版、および最新のドライバが\n"
			L"インストールされているかどうかご確認ください。\n";
		if (!fixed_size) {
			s+=
				L"また、起動時のオプションで解像度を変更してみてください。\n\n"
				L"ウィンドウ時800x600、フルスクリーン1024x768の場合：\n"
				L"-w 800 -h 600 -fw 1024 -fh 768";
		}
		MessageBox(HWnd, s.c_str(), app_name, MB_OK);
		exit(1);
	}
	Graphics->Clear();
	Graphics->Present();
	if (FullScreen) ResetScreen();

	// 入力の初期化
	Input=new CInput(HWnd);
	Mouse.X=Mouse.Y=0;
	Mouse.LButton=Mouse.MButton=Mouse.RButton=false;

	// 時間
	LastPerfCounter.QuadPart=0;
	LastTickCount=0;
	Elapsed=0;

	// メニュー
	if (UseMenu) {
		MenuBar=CreateMenu();
		SetMenu(HWnd, MenuBar);
	}
}


//==============================================================
static const char* copyright=
	"Copyright(C) 2005 ひぐぺん工房（松浦健一郎／司ゆき）";


//==============================================================
// メニュー
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
// ＜CGame＞デストラクタ
CGame::~CGame() {
	delete Input;
	delete Graphics;
}


//==============================================================
// ＜CGame＞時刻のリセット
void CGame::ResetTime() {
	LARGE_INTEGER freq, count;
	if (QueryPerformanceFrequency(&freq) && QueryPerformanceCounter(&count)) {
		LastPerfCounter=count;
	} else {
		LastTickCount=GetTickCount();
	}
}

// 画面のリセット
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

// ウィンドウモードとフルスクリーンモードの切り替え
void CGame::ToggleFullScreen() {
	FullScreen=!Graphics->IsFullScreen();
	ResetScreen();
}

//==============================================================
// ＜CGame＞ゲームの実行
void CGame::Run() {

	// デバイスリセット後の処理を呼び出す
	OnResetDevice();

	// ダミーのメッセージハンドラを，
	// 正式なメッセージハンドラに置き換える
	SetWindowLong(HWnd, GWL_WNDPROC, (long)::WndProc);
	
	// メッセージループ
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ResetTime();
	while (msg.message!=WM_QUIT) {

		// バックグラウンド時のメッセージ処理
		if (PauseInTheBackground && HWnd!=GetForegroundWindow()) {
			GetMessage(&msg, NULL, 0U, 0U);
			if (!TranslateAccelerator(HWnd, HAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			ResetTime();
			continue;
		}
		
		// フォアグラウンド時のメッセージ処理
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			if (!TranslateAccelerator(HWnd, HAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			continue;
		}

		// フルスクリーン時のメニュー表示
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

		// 一時停止
		if (Pause || MenuPause) continue;

		// 時間調整
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

		// 移動と描画
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

	// メッセージハンドラをダミーに戻す
	SetWindowLong(HWnd, GWL_WNDPROC, (long)::DummyWndProc);
	
	// 終了処理
	UnregisterClass(AppName.c_str(), GetModuleHandle(NULL));
}

// 描画
void CGame::DrawScene() {

	// デバイスが消失していたら再構築する
	switch (Graphics->GetDevice()->TestCooperativeLevel()) {
		case D3DERR_DEVICELOST:
			Sleep(50);
			return;
		case D3DERR_DEVICENOTRESET:
			OnLostDevice();
			if (Graphics->ResetDevice()) OnResetDevice();
			return;
	}

	// 描画開始
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();
	Graphics->BeginScene();

	Draw();

	// 描画完了
	Graphics->EndScene();
	Graphics->Present();
}

