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
// �O���t�B�b�N�X�`��@�\���Ǘ�����N���X
class CGraphics {
protected:

	// Direct3D�C���^�t�F�[�X�A�f�o�C�X
	LPDIRECT3D9 D3D;
	LPDIRECT3DDEVICE9 Device;

	// �E�B���h�E�n���h��
	HWND HWnd;
	
	// ���̃E�B���h�E�X�^�C��
	DWORD WindowStyle;

	// ���A����
	int Width, Height;

	// �t���X�N���[�����[�h�Ȃ��true
	bool FullScreen;

	// ���t���b�V�����[�g(Hz)
	int RefreshRate;

	// GDI�ɂ��`�悪�\�ȃT�[�t�F�X���ǂ���
	bool IsGDISurface;

	D3DFORMAT DepthStencilFormat;

public:

	// �R���X�g���N�^�C�f�X�g���N�^
	CGraphics(HWND hwnd);
	~CGraphics();

	// �f�o�C�X�̃��Z�b�g
	void SetWidth(int w) { Width=w; }
	void SetHeight(int h) { Height=h; }
	void SetFullScreen(bool b) { FullScreen=b; }
	void SetRefreshRate(int r) { RefreshRate=r; }
	void SetDepthStencilFormat(D3DFORMAT format) { DepthStencilFormat=format; }
	bool ResetDevice();
	bool DeviceNotReset();

	// �V�[���`��̊J�n�C�I��
	bool BeginScene() { return SUCCEEDED(Device->BeginScene()); }
	void EndScene() { Device->EndScene(); }

	// ��ʏ����C��ʃt���b�v
	void Clear(D3DCOLOR color=D3DCOLOR_ARGB(255, 0, 0, 0));
	bool Present();

	// �e����̎擾
	LPDIRECT3DDEVICE9 GetDevice() { return Device; }
	HWND GetHWnd() { return HWnd; }
	int GetWidth() { return Width; }
	int GetHeight() { return Height; }
	bool IsFullScreen() { return FullScreen; }
	int GetRefreshRate() { return RefreshRate; }

	// GDI�ɂ��`�悪�\�ȃT�[�t�F�X�ɂ���
	void PresentGDISurface() { if (!IsGDISurface) Present(); }

	// ���C�g�̐ݒ�
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

	// �A���t�@���Z���[�h�̎w��
	void SetAddAlpha(bool add_alpha);

	// �G�t�F�N�g�̃��[�h
	ID3DXEffect* LoadEffect(wstring file);
	
	// �e�N�X�`���̃��[�h
	LPDIRECT3DTEXTURE9 LoadTexture(wstring file);
};


#endif



