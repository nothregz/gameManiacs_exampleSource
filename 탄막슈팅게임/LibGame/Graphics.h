#ifndef GRAPHICSH
#define GRAPHICSH

#include <d3dx9.h>
#include <string>
using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif


//==============================================================
// グラフィックス描画機能を管理するクラス
class CGraphics {
protected:

	// Direct3Dインタフェース、デバイス
	LPDIRECT3D9 D3D;
	LPDIRECT3DDEVICE9 Device;

	// ウィンドウハンドル
	HWND HWnd;
	
	// 元のウィンドウスタイル
	DWORD WindowStyle;

	// 幅、高さ
	int Width, Height;

	// フルスクリーンモードならばtrue
	bool FullScreen;

	// リフレッシュレート(Hz)
	int RefreshRate;

	// GDIによる描画が可能なサーフェスかどうか
	bool IsGDISurface;

	D3DFORMAT DepthStencilFormat;

public:

	// コンストラクタ，デストラクタ
	CGraphics(HWND hwnd);
	~CGraphics();

	// デバイスのリセット
	void SetWidth(int w) { Width=w; }
	void SetHeight(int h) { Height=h; }
	void SetFullScreen(bool b) { FullScreen=b; }
	void SetRefreshRate(int r) { RefreshRate=r; }
	void SetDepthStencilFormat(D3DFORMAT format) { DepthStencilFormat=format; }
	bool ResetDevice();
	bool DeviceNotReset();

	// シーン描画の開始，終了
	bool BeginScene() { return SUCCEEDED(Device->BeginScene()); }
	void EndScene() { Device->EndScene(); }

	// 画面消去，画面フリップ
	void Clear(D3DCOLOR color=D3DCOLOR_ARGB(255, 0, 0, 0));
	bool Present();

	// 各種情報の取得
	LPDIRECT3DDEVICE9 GetDevice() { return Device; }
	HWND GetHWnd() { return HWnd; }
	int GetWidth() { return Width; }
	int GetHeight() { return Height; }
	bool IsFullScreen() { return FullScreen; }
	int GetRefreshRate() { return RefreshRate; }

	// GDIによる描画が可能なサーフェスにする
	void PresentGDISurface() { if (!IsGDISurface) Present(); }

	// ライトの設定
	void SetLight(
		int id, D3DLIGHTTYPE type, 
		D3DXVECTOR3 pos, D3DXVECTOR3 dir, D3DXCOLOR color, 
		float range
	);
	void SetLight(
		int id, D3DLIGHTTYPE type, 
		float x, float y, float z,
		float range=1000
	);

	// アルファ加算モードの指定
	void SetAddAlpha(bool add_alpha);

	// エフェクトのロード
	ID3DXEffect* LoadEffect(wstring file);
	
	// テクスチャのロード
	LPDIRECT3DTEXTURE9 LoadTexture(wstring file);
};


#endif



