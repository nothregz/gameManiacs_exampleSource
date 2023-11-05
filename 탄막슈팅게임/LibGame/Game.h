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


// �Q�[���{�̂̃N���X
class CGame {
protected:

	// �A�N�Z�����[�^�L�[
	HACCEL HAccel;

	// �E�B���h�E�n���h��
	HWND HWnd;

	// �O���t�B�b�N�X
	CGraphics* Graphics;

	// ����
	CInput* Input;
	MOUSE_STATE Mouse;

	// ���s�t�@�C���̃p�X�A�A�v���P�[�V�������A�w���v��URL
	wstring ExePath, AppName, HelpURL;

	// ���x����
	LARGE_INTEGER LastPerfCounter;
	DWORD LastTickCount;
	double Elapsed;
	void ResetTime();

	// �ꎞ��~
	bool Pause, MenuPause;

	// ���j���[
	bool UseMenu;
	HMENU MenuBar;

	// �I���m�F
	bool ConfirmExit;

	// ��ʃT�C�Y�A���t���b�V�����[�g
	int WindowWidth, WindowHeight, FullScreenWidth, FullScreenHeight,
		RefreshRate;
	bool FullScreen;
	
	void DrawScene();

public:

	// �R���X�g���N�^�C�f�X�g���N�^
	CGame(const wchar_t* app_name, bool zbuffer, bool use_menu, bool fixed_size);
	~CGame();

	// ���b�Z�[�W�n���h��
	virtual LRESULT WINAPI WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

	// �Q�[���̎��s
	void Run();

	// �e��ݒ�
	double FPS;
	bool DropFrames, PauseInTheBackground;

	// �ړ��ƕ`��F
	// �T�u�N���X�ŃI�[�o�[���C�h����
	virtual void Move() {}
	virtual void Draw() {}
	
	// Direct3D���\�[�X�̊Ǘ�
	virtual void OnLostDevice() {}
	virtual void OnResetDevice() {}

	// ���j���[
	HMENU GetMenuBar() { return MenuBar; }
	HMENU AddMenu(wstring text);
	HMENU AddSubMenu(HMENU menu, wstring text);
	void AddMenuItem(HMENU menu, wstring text, int id);
	void AddMenuSeparator(HMENU menu);
	void CheckMenuItem(HMENU menu, int id, bool check);

	// �e����̎擾
	CGraphics* GetGraphics() { return Graphics; }
	CInput* GetInput() { return Input; }
	const MOUSE_STATE& GetMouseState() { return Mouse; }

	// �I���m�F
	void SetConfirmExit(bool ce) { ConfirmExit=ce; }

	// ��ʃ��[�h�̐؂�ւ�
	void ResetScreen();
	void ToggleFullScreen();
};


#endif
