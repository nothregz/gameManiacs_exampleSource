#pragma warning(disable:4819)

#include "Graphics.h"
#include <d3dx9.h>


//==============================================================
// コンストラクタ
CGraphics::CGraphics(HWND hwnd)
:	HWnd(hwnd), FullScreen(false), RefreshRate(60),
	Device(NULL), IsGDISurface(true),
	DepthStencilFormat(D3DFMT_D16)
{
	// ウィンドウスタイルの保存
	WindowStyle=GetWindowLong(hwnd, GWL_STYLE);

	// クライアント領域のサイズを取得
	RECT r;
	GetClientRect(hwnd, &r);
	Width=r.right-r.left;
	Height=r.bottom-r.top;

	// Direct3Dインタフェースの作成
	D3D=Direct3DCreate9(D3D_SDK_VERSION);

	// Direct3Dデバイスの作成
	ResetDevice();
}


//==============================================================
// デストラクタ
CGraphics::~CGraphics() {
	if (Device) Device->Release();
	if (D3D) D3D->Release();
}


//==============================================================
// デバイス

// デバイスのリセット
bool CGraphics::ResetDevice() {

	// ウィンドウスタイルとサイズの設定：
	// デバイスリセットより先に行わないとウィンドウサイズが変わる
	if (FullScreen) {
		SetWindowLong(HWnd, GWL_STYLE, WS_VISIBLE);
	} else {
		SetWindowLong(HWnd, GWL_STYLE, WindowStyle);
		RECT r={0, 0, Width, Height};
		AdjustWindowRect(&r, WindowStyle, GetMenu(HWnd)!=NULL);
		SetWindowPos(HWnd, HWND_NOTOPMOST, 
			0, 0, r.right-r.left, r.bottom-r.top, SWP_SHOWWINDOW);
	}

	// デバイスのパラメータ設定
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.hDeviceWindow=HWnd;
	d3dpp.BackBufferWidth=Width;
	d3dpp.BackBufferHeight=Height;
	d3dpp.Windowed=!FullScreen;
	d3dpp.FullScreen_RefreshRateInHz=FullScreen?RefreshRate:0;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat=D3DFMT_A8R8G8B8;
	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_ONE;
	d3dpp.EnableAutoDepthStencil=TRUE;
	d3dpp.AutoDepthStencilFormat=DepthStencilFormat;

	// デバイスの作成またはリセット
	// GeForce系でスキニングを正常に動作させるには
	// D3DCREATE_MIXED_VERTEXPROCESSINGにする必要がある
	if (!Device) {
		if (FAILED(D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&d3dpp, &Device))
		) 
		if (FAILED(D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp, &Device))
		)
		if (FAILED(D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &Device))
		)
		if (FAILED(D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, HWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &Device))
		) return false;
	} else {
		if (FAILED(Device->Reset(&d3dpp))) return false;
	}

	// 幅と高さの取得
	LPDIRECT3DSURFACE9 back_buffer;
	if (SUCCEEDED(Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer))) {
		D3DSURFACE_DESC desc;
		back_buffer->GetDesc(&desc);
		Width=desc.Width;
		Height=desc.Height;
		back_buffer->Release();
	} else {
		Width=Height=0;
	}

	// リフレッシュレートの取得
	D3DDISPLAYMODE mode;
	if (SUCCEEDED(Device->GetDisplayMode(0, &mode))) {
		RefreshRate=mode.RefreshRate;
	} else {
		RefreshRate=0;
	}

	IsGDISurface=true;
	return true;
}


//==============================================================
// 画面（バックバッファ）の消去
void CGraphics::Clear(D3DCOLOR color) {
	Device->Clear(0, NULL, 
		D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, color, 1, 0 );
}


//==============================================================
// 画面の更新
bool CGraphics::Present() {
	if (FAILED(Device->Present(0, 0, 0, 0))) return false;
	IsGDISurface=!IsGDISurface;
	return true;
}


//==============================================================
// ライトの初期化
void CGraphics::SetLight(
	int id, D3DLIGHTTYPE type, 
	D3DXVECTOR3 pos, D3DXVECTOR3 dir, D3DXCOLOR color, 
	float range
) {
	// ライトの作成
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type=type;
	light.Position=pos;
	D3DXVec3Normalize(&dir, &dir);
	light.Direction=dir;
	light.Diffuse=color, 
	light.Range=range;

	// ライトの設定
	Device->SetLight(id, &light);
	Device->LightEnable(id, TRUE);
	Device->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CGraphics::SetLight(
	int id, D3DLIGHTTYPE type, 
	float x, float y, float z,
	float range
) {
	SetLight(
		id, type, 
		D3DXVECTOR3(x, y, z), D3DXVECTOR3(x, y, z), 
		D3DXCOLOR(1, 1, 1, 1), range
	);
}

//==============================================================
// アルファ加算モードの設定
void CGraphics::SetAddAlpha(bool add_alpha) {
	if (add_alpha) {
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	} else {
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}

//==============================================================
// エフェクトのロード
ID3DXEffect* CGraphics::LoadEffect(wstring file) {
	ID3DXEffect* effect=NULL;
	LPD3DXBUFFER error;

	// エフェクトファイルの存在を確認する
	WIN32_FIND_DATA fd;
	if (FindFirstFile(file.c_str(), &fd)==INVALID_HANDLE_VALUE) {
		MessageBox(NULL, file.c_str(), L"Shader File Not Found", MB_OK);
		PostQuitMessage(1);
		return NULL;
	}

	// エフェクトファイルのロード
	D3DXCreateEffectFromFile(
		Device, file.c_str(), 
		NULL, NULL, 0, NULL, &effect, &error);

	// コンパイルエラーの場合はエラーを表示して終了
	if (!effect) {
		MessageBox(
			NULL, 
			error?(LPCTSTR)error->GetBufferPointer():NULL, 
			L"Shader Compile Error", MB_OK);
		PostQuitMessage(1);
		return NULL;
	}
	return effect;
}

//==============================================================
// テクスチャのロード
LPDIRECT3DTEXTURE9 CGraphics::LoadTexture(wstring file) {
	LPDIRECT3DTEXTURE9 texture=NULL;
	D3DXCreateTextureFromFile(Device, file.c_str(), &texture);
	return texture;
}

