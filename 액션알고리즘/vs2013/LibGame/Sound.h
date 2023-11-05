#ifndef LIBGAME_SOUNDH
#define LIBGAME_SOUNDH

// DX9 (2010) 부터는 DirectMusic이 빠졌다.
// 따라서 해당 기능을 쓰는 코드를 제거하기 위하여 USE_DMUSIC define을 선언함
#define USE_DMUSIC	0

#include "LibUtil\Archive.h"
#include <dmusicc.h>
#if USE_DMUSIC
#include <dmusici.h>
#endif
#include <vector>
#include <string>
using namespace std;


// 효과음 데이터 클래스
class CSound {
friend class CSoundPlayer;
protected:

#if USE_DMUSIC
	IDirectMusicSegment8* Segment;
#endif
	BYTE* MemData;

public:

	CSound(string file, CArchiveLoader* archive);
	~CSound();
	void SetRepeats(unsigned int repeats);

};


// 효과음 재생 클래스
class CSoundPlayer {
protected:

#if USE_DMUSIC
	IDirectMusicPerformance8* Performance;
	IDirectMusicAudioPath8* AudioPath;
#endif
	IDirectSound3DBuffer8* DS3DBuffer;

public:

	CSoundPlayer(bool use3d, int channels);
	~CSoundPlayer();

	void Set3DPosition(float x, float y, float z);
	void SetVolume(float volume);

	bool IsPlaying(CSound* sound);
	void Download(CSound* sound);
	void Play(CSound* sound);
	void Stop(CSound* sound=NULL);

};


#endif
