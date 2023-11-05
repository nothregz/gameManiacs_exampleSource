#include "Sound.h"
#include "..\..\LibUtil\LibUtil\LibUtil.h"
#include <windows.h>
#include <cguid.h>
#include <math.h>


//==============================================================
// 사운드 관련 공통 데이터
class CSoundCommon {
public:
	IDirectMusicLoader8* Loader;
	unsigned int NumSounds;
	CSoundCommon();
	~CSoundCommon();
};
static CSoundCommon* SoundCommon=NULL;

// 생성자
CSoundCommon::CSoundCommon()
:	Loader(NULL), NumSounds(0)
{
	CoInitialize(NULL);
	CoCreateInstance(
		CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
		IID_IDirectMusicLoader8, (void**)&Loader);
}

// 공통 데이터의 소멸자
CSoundCommon::~CSoundCommon() {
	if (Loader) Loader->Release();
	CoUninitialize();
}


//==============================================================
// 효과음 데이터의 클래스

// 생성자
CSound::CSound(string file, CArchiveLoader* archive)
:	Segment(NULL), MemData(NULL)
{
	if (!SoundCommon) SoundCommon=new CSoundCommon();
	SoundCommon->NumSounds++;

	// 로더 확인
	if (!SoundCommon->Loader) return;

	// 파일명 가공
	WCHAR wc_file[MAX_PATH+1];
	if (!MultiByteToWideChar(CP_ACP, 0, file.c_str(), -1, wc_file, MAX_PATH)) return;
	
	// 음성데이터 로드
	if (archive) {
		ARCHIVE_ENTRY* e=archive->Find(file);
		if (!e) return;
		
		// GetObject는 임시메모리 영역을 전달할 수 없기 때문에 
		// 메모리 영역을 확보하여 데이터를 복사한다.
		MemData=new BYTE[e->Size];
		memcpy(MemData, e->Data, e->Size);
		
		DMUS_OBJECTDESC desc;
		desc.dwSize=sizeof(DMUS_OBJECTDESC);
		desc.guidClass=CLSID_DirectMusicSegment;
		desc.dwValidData=DMUS_OBJ_CLASS|DMUS_OBJ_MEMORY;
		desc.pbMemData=MemData;
		desc.llMemLength=e->Size;
		SoundCommon->Loader->GetObject(
			&desc, IID_IDirectMusicSegment8, (LPVOID*)&Segment);
	} else {
		SoundCommon->Loader->LoadObjectFromFile(
			CLSID_DirectMusicSegment, IID_IDirectMusicSegment8,
			wc_file, (LPVOID*)&Segment);
	}
}

// 소멸자
CSound::~CSound()
{
	// 세그먼트 해방
	if (Segment) Segment->Release();
	if (MemData) delete[] MemData;
	
	// 로더 삭제
	SoundCommon->NumSounds--;
	if (!SoundCommon->NumSounds) {
		delete SoundCommon;
		SoundCommon=NULL;
	}
}

// 재생횟수 설정
void CSound::SetRepeats(unsigned int repeats) {
	if (!Segment) return;
	repeats=
		repeats<0 ? DMUS_SEG_REPEAT_INFINITE : 
		repeats>0 ? (DWORD)repeats-1 : 0;
	Segment->SetRepeats(repeats);	
}


//==============================================================
// 효과음 재생 클래스

// 생성자
CSoundPlayer::CSoundPlayer(bool use3d, int channels)
:	Performance(NULL), AudioPath(NULL), DS3DBuffer(NULL)
{
	if (!SoundCommon) SoundCommon=new CSoundCommon();

	// 퍼포먼스 작성
	CoCreateInstance(
		CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
		IID_IDirectMusicPerformance8, (void**)&Performance);

	// 퍼포먼스 초기화, 오디오 경로 취득
	if (!Performance) return;
	Performance->InitAudio(
		NULL, NULL, NULL, 
		use3d?DMUS_APATH_DYNAMIC_3D:DMUS_APATH_DYNAMIC_STEREO,
		channels,
		DMUS_AUDIOF_ALL, NULL);
	Performance->GetDefaultAudioPath(&AudioPath);
	
	// DirectSound3D 버퍼 취득
	if (!AudioPath) return;
	if (use3d) {
		AudioPath->GetObjectInPath(
			DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, 
			GUID_NULL, 0, IID_IDirectSound3DBuffer, 
			(LPVOID*)&DS3DBuffer);
	}
}

// 소멸자
CSoundPlayer::~CSoundPlayer()
{
	if (DS3DBuffer) DS3DBuffer->Release();
	if (Performance) {
		Performance->CloseDown();
		Performance->Release();
	}
}

// 3D 공간에서 재생 위치 설정
void CSoundPlayer::Set3DPosition(float x, float y, float z) {
	if (!DS3DBuffer) return;
	DS3DBuffer->SetPosition(x, y, z, DS3D_IMMEDIATE );
}

// 볼륨 설정 (0.0~1.0)
void CSoundPlayer::SetVolume(float volume) {
	if (!AudioPath) return;
	volume=100*20*log10(volume);
	long v=0;
	
	if (volume<-9600) v=-9600; else if (volume>0) v=0; else v=(long)volume;
	AudioPath->SetVolume(v, 0);
}

// 다운로드
void CSoundPlayer::Download(CSound* sound) {
	sound->Segment->Download(Performance);
}

// 재생
void CSoundPlayer::Play(CSound* sound) {
	if (!sound->Segment || !Performance || !AudioPath) return;
	Performance->PlaySegmentEx(
		sound->Segment, NULL, NULL, DMUS_SEGF_SECONDARY, 
		0, NULL, NULL, AudioPath);
}

// 정지
void CSoundPlayer::Stop(CSound* sound) {
	if (!Performance) return;
	Performance->Stop(sound->Segment, NULL, 0, 0);
}

// 재생 중인지 여부
bool CSoundPlayer::IsPlaying(CSound* sound) {
	if (!Performance) return false;
	return Performance->IsPlaying(sound->Segment, NULL)==S_OK;
}

