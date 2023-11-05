#ifndef LIBGAME_SOUNDH
#define LIBGAME_SOUNDH

#include "..\..\LibUtil\LibUtil\Archive.h"
#include <dmusicc.h>
#include <dmusici.h>
#include <vector>
#include <string>
using namespace std;


// 효과음 데이터 클래스
class CSound {
friend class CSoundPlayer;
protected:

	IDirectMusicSegment8* Segment;
	BYTE* MemData;

public:

	CSound(string file, CArchiveLoader* archive);
	~CSound();
	void SetRepeats(unsigned int repeats);

};


// 효과음 재생 클래스
class CSoundPlayer {
protected:

	IDirectMusicPerformance8* Performance;
	IDirectMusicAudioPath8* AudioPath;
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
