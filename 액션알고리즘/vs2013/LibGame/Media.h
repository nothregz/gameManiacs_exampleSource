#ifndef SHOWH
#define SHOWH

#include <DShow.h>
#include <string>
using namespace std;


//==============================================================
// 동영상 및 오디오 재생
class CMedia {
private:

	// 그래프 빌더 (미디어소스와 디코더 관리)
	// 미디어 컨트롤 (재생 및 정지)
	// 미디어 이벤트 (재생 시의 이벤트 처리)
	// 미디어 seeking (재생 위치의 취득 및 지정)
	IGraphBuilder* Graph;
	IMediaControl* Control;
	IMediaEvent* Event;
	IMediaSeeking* Seeking;
	IBasicAudio* Audio;

    void CleanUp();

public:

	// 생성자, 소멸자
	CMedia();
	~CMedia();
	
	// 재생, 일시정지, 정지
	bool LoadFromFile(string file_name);
	void Play();
	void Pause();
	void Stop();

	// 볼륨 (0.0~1.0)
	float GetVolume();
	void SetVolume(float vol);

	// 밸런스 (-1.0~1.0)
	float GetBalance();
	void SetBalance(float bal);

	// 재생이 종료했는지 여부
	bool IsFinished();

	// 재생 중인 위치, 전체 길이 (초 단위)
	float GetPos();
	float GetLength();
	void MovePos(float offset);
	void SetPos(float pos);
};


#endif

