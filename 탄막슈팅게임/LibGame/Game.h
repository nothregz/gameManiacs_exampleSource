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


// ゲーム本体のクラス
class CGame {
protected:

	// アクセラレータキー
	HACCEL HAccel;

	// ウィンドウハンドル
	HWND HWnd;

	// グラフィックス
	CGraphics* Graphics;

	// 入力
	CInput* Input;
	MOUSE_STATE Mouse;

	// 実行ファイルのパス、アプリケーション名、ヘルプのURL
	wstring ExePath, AppName, HelpURL;

	// 速度調整
	LARGE_INTEGER LastPerfCounter;
	DWORD LastTickCount;
	double Elapsed;
	void ResetTime();

	// 一時停止
	bool Pause, MenuPause;

	// メニュー
	bool UseMenu;
	HMENU MenuBar;

	// 終了確認
	bool ConfirmExit;

	// 画面サイズ、リフレッシュレート
	int WindowWidth, WindowHeight, FullScreenWidth, FullScreenHeight,
		RefreshRate;
	bool FullScreen;
	
	void DrawScene();

public:

	// コンストラクタ，デストラクタ
	CGame(const wchar_t* app_name, bool zbuffer, bool use_menu, bool fixed_size);
	~CGame();

	// メッセージハンドラ
	virtual LRESULT WINAPI WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

	// ゲームの実行
	void Run();

	// 各種設定
	double FPS;
	bool DropFrames, PauseInTheBackground;

	// 移動と描画：
	// サブクラスでオーバーライドする
	virtual void Move() {}
	virtual void Draw() {}
	
	// Direct3Dリソースの管理
	virtual void OnLostDevice() {}
	virtual void OnResetDevice() {}

	// メニュー
	HMENU GetMenuBar() { return MenuBar; }
	HMENU AddMenu(wstring text);
	HMENU AddSubMenu(HMENU menu, wstring text);
	void AddMenuItem(HMENU menu, wstring text, int id);
	void AddMenuSeparator(HMENU menu);
	void CheckMenuItem(HMENU menu, int id, bool check);

	// 各種情報の取得
	CGraphics* GetGraphics() { return Graphics; }
	CInput* GetInput() { return Input; }
	const MOUSE_STATE& GetMouseState() { return Mouse; }

	// 終了確認
	void SetConfirmExit(bool ce) { ConfirmExit=ce; }

	// 画面モードの切り替え
	void ResetScreen();
	void ToggleFullScreen();
};


#endif
