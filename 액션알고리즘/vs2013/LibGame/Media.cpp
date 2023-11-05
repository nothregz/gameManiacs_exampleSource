#pragma warning(disable:4996)
#pragma warning(disable:4995)

#include "Media.h"
#include <stdlib.h>


//==============================================================
// 생성자
CMedia::CMedia()
:	Graph(NULL), Control(NULL), Event(NULL), 
	Seeking(NULL), Audio(NULL)
{
	// COM 라이브러리의 초기화
	CoInitialize(NULL);
}


//==========================================================
// 소멸자
CMedia::~CMedia() {
	CleanUp();
	CoUninitialize();
}


//==========================================================
// 취득한 자원의 해방
void CMedia::CleanUp() {
	if (Seeking) { Seeking->Release(); Seeking=NULL; }
	if (Event)   { Event->Release();   Event=NULL;   }
	if (Control) { Control->Release(); Control=NULL; }
	if (Graph)   { Graph->Release();   Graph=NULL;   }
	if (Audio)   { Audio->Release();   Audio=NULL;   }
}


//==============================================================
// 파일 로드
bool CMedia::LoadFromFile(string file_name) {

	// 자원의 해방
	CleanUp();

	// 인터페이스 취득
	// （GraphBuilder，MediaControl，MediaEvent，MediaSeeking）
	if (FAILED(CoCreateInstance(
			CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
			IID_IGraphBuilder, (void **)&Graph)) ||
		FAILED(Graph->QueryInterface(
			IID_IMediaControl, (void **)&Control)) ||
		FAILED(Graph->QueryInterface(
			IID_IMediaEvent, (void **)&Event)) ||
		FAILED(Graph->QueryInterface(
			IID_IMediaSeeking, (void **)&Seeking)) ||
		FAILED(Graph->QueryInterface(
			IID_IBasicAudio, (void **)&Audio))
	) return false;
	
	// 파일명을 Unicode로 변환:
	// mbstowcs 함수에서는 잘 움직이지 않는다.
	wchar_t fn[MAX_PATH];
	MultiByteToWideChar(
		0, 0, file_name.c_str(), -1, fn, MAX_PATH);

	// 그래프 작성
	if (FAILED(Graph->RenderFile(fn, NULL))) return false;

	return true;
}


//==============================================================
// 정지
void CMedia::Stop() {
	if (!Control) return;
	Control->Stop();
	SetPos(0);
}


//==============================================================
// 재생, 일시정지

// 재생
void CMedia::Play() {
	if (!Control) return;
	Control->Run();
}

// 일시정지
void CMedia::Pause() {
	if (!Control) return;
	Control->Pause();
}


//==============================================================
// 재생이 끝났는지 여부
bool CMedia::IsFinished() {
	if (!Event) return true;
	long code;
	return SUCCEEDED(Event->WaitForCompletion(0, &code));
}


//==============================================================
// 재생중인 위치, 전체 길이 (초 단위)
float CMedia::GetPos() {
	if (!Seeking) return 0;
	LONGLONG current, length;
	Seeking->GetPositions(&current, &length);
	double d=(double)current;
	return (float)(d/10000000);
}

float CMedia::GetLength() {
	if (!Seeking) return 0;
	LONGLONG current, length;
	Seeking->GetPositions(&current, &length);
	double d=(double)length;
	return (float)(d/10000000);
}

//==============================================================
// 재생 위치 변경

// 절대 위치
void CMedia::SetPos(float pos) {
	if (!Seeking) return;
	LONGLONG current=(LONGLONG)((double)pos*10000000);
	Seeking->SetPositions(
		&current, AM_SEEKING_AbsolutePositioning,
		NULL, AM_SEEKING_NoPositioning);
}

// 상대 위치
void CMedia::MovePos(float offset) {
	SetPos(GetPos()+offset);
}

//==============================================================
// 볼륨 (0.0~1.0)

float CMedia::GetVolume() {
	if (!Audio) return 0;
	long l;
	Audio->get_Volume(&l);
	return ((float)l+10000)/10000;
}

void CMedia::SetVolume(float vol) {
	if (!Audio) return;
	long l=(long)(vol*10000-10000);
	if (l<-10000) l=-10000; else if (l>0) l=0;
	Audio->put_Volume(l);
}

//==============================================================
// 밸런스 (-1.0~1.0)

float CMedia::GetBalance() {
	if (!Audio) return 0;
	long l;
	Audio->get_Balance(&l);
	return (float)l/10000;
}

void CMedia::SetBalance(float bal) {
	if (!Audio) return;
	long l=(long)(bal*10000);
	if (l<-10000) l=-10000; else if (l>10000) l=10000;
	Audio->put_Balance(l);
}


