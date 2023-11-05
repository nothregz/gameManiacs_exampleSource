#include "Main.h"
#include "Mover.h"

#include <string>
using namespace std;

// 게임 본체
CActGame* Game;

// 텍스처 파일명
static const char* TEX_NAME[]={
	"Coin", "Enemy0", "Enemy1", "Fish", 
	"Floor", "IceFloor", "Item", "Lift", 
	"Man", "Swimmer", "Tree", "Loop", 
	"StampBoard", "Rope", "Ladder",
	"Trampoline0", "Trampoline1", "Trampoline2", 
	"DropFloor", "RevolvingDoor", 
	"Door0", "Door1", "Door2", "Door3", 
	"Elevator0", "Elevator1", 
	"MovingFloor", "Conveyor0", "Conveyor1", 
	"AirCurrent0", "AirCurrent1", 
	"WarpGate", "Rock", "Clay", "Object",
	"Ice", "Hole", "Foothold", "Panel", 
	"Pile", "Crack", 
	"Skateboard", "Car", "Animal", 
	"Seesaw0", "Seesaw1", "Crouch",
	"Sword", "Pepper", "Bomb", "Light", 
	"Shuriken", "Fill", "Ball", "Fire", "Blast", 
	"Missile", "Bullet", "Boomerang", "Shield", 
	"Bubble", "Medicine", "Heart", "Mushroom", "Jewel", 
};

// 생성자(constructor)
CActGame::CActGame()
:	CGame("도서 [게임 매니악스 액션 게임 알고리즘] 샘플 프로그램", true, false, true), 
	Paused(false), PrevInput(false),
	StageIndex(0), StageActive(false)
{
	// 도움말, 종료, 커서와 관련한 설정
	HelpURL=ExePath+"..\\Help\\index.html";
	ConfirmExit=false;
	ShowCursor(TRUE);
	FPS=60;
	PauseInTheBackground=true;
	DropFrames=false;

	// 폰트 초기화
	static const char fontchar[]=
		"~|0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.(){}<>!?+-x*/='\\\"#$%&@[]";
	Font=new CFont(Graphics->GetDevice(), ExePath+"..\\Font\\font.png", NULL, fontchar, 16.0f/256, 32.0f/256, 16, 32, 2, 2);

	// 텍스처 초기화
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();
	for (int i=0; i<TEX_COUNT; i++) {
		string path=ExePath+"..\\Texture\\"+TEX_NAME[i]+".png";
		Texture.push_back(new CTexture(Graphics->GetDevice(), path, NULL));
	}

	// 작업목록 초기화
	MoverList=new CTaskList(200, 1000);
}

// 소멸자(destructor)
CActGame::~CActGame() {
	delete Font;
	for (size_t i=0, n=Texture.size(); i<n; i++) delete Texture[i];
}

// 스테이지 설정
void CActGame::SetStage(int stage) {
	MoverList->DeleteTask();
	StageIndex=(stage+Stage.size())%Stage.size();
	Stage[StageIndex]->Init();
}

// 동작
void CActGame::Move() {

	// 입력
	Input->UpdateState();
	const CInputState* is=Input->GetState(0);

	// 일시정지(스크린샷 용)
	if (!PrevInput && is->Button[3]) Paused=!Paused;

	// 스테이지 동작
	if (!Paused) {
		if (StageActive) {
			StageActive=Stage[StageIndex]->Move(is);
			for (CTaskIter i(MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (!mover->Move(is)) i.Remove();
			}
			if (!PrevInput && is->Button[2]) StageActive=false;
		} else {
			if (!PrevInput) {
				if (is->Button[2]) {
					StageActive=true;
					SetStage(StageIndex);
				} else
				if (is->Left) SetStage(StageIndex+1); else
				if (is->Right) SetStage(StageIndex-1); else
				if (is->Up) SetStage(StageIndex+10); else
				if (is->Down) SetStage(StageIndex-10);
			}
		}
	}
	
	// 이전 입력
	PrevInput=is->Button[2]||is->Button[3];
	if (!StageActive) PrevInput=PrevInput||is->Left||is->Right||is->Up||is->Down;
}

// 그리기
void CActGame::Draw() {
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();
	Graphics->Clear(COL_WHITE);
	for (int order=0, count=0; count<MoverList->GetNumActiveTask(); order++) {
		for (CTaskIter i(MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (order==mover->DrawOrder) {
				mover->Draw();
				count++;
			}
		}
	}
	CStage* stage=Stage[StageIndex];
	stage->Draw();
	Font->DrawText(string("\"")+stage->Name+"\"", 0, 0, COL_BLACK);//, COL_SHADE);
	if (!StageActive) {
		Font->DrawText("PUSH BUTTON2 OR [C] TO START", 0, 32, COL_BLACK);//, COL_SHADE);
	}
}

// 디바이스 리셋
void CActGame::OnResetDevice() {
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();

	// 알파 블렌딩 설정
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 렌더링 설정
	device->SetRenderState(D3DRS_DITHERENABLE, FALSE);
	device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_AMBIENT, 0x006F6F6F);
	device->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
	device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	// 텍스처 처리방법 설정
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	
	// 월드 매트릭스
	D3DXMATRIX mat_world;
	D3DXMatrixIdentity(&mat_world);
	device->SetTransform(D3DTS_WORLD, &mat_world);

	// 시야 매트릭스
	D3DXMATRIX mat_view;
	D3DXVECTOR3 vec_from=D3DXVECTOR3(0, 100, 0);
	D3DXVECTOR3 vec_lookat=D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 vec_up=D3DXVECTOR3(0, 0, 1);
	D3DXMatrixLookAtLH(&mat_view, &vec_from, &vec_lookat, &vec_up);
	device->SetTransform(D3DTS_VIEW, &mat_view);
	
	// 투영 매트릭스
	D3DXMATRIX mat_proj;
	D3DXMatrixOrthoLH(&mat_proj, 100, 100, 1, 1000);
	device->SetTransform(D3DTS_PROJECTION, &mat_proj);

	// 라이팅
	Graphics->SetLight(0, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, -1.0f, 1000);
}

// 메인 루틴
INT WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, INT) {
	Game=new CActGame();
	MakeStage();
	Game->SetStage(-1);
	Game->Run();
	delete Game;
	return 0;
}

