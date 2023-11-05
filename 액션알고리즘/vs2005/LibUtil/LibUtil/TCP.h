#ifndef TCPH
#define TCPH

#include <winsock.h>
#include <string>
#include <vector>
using namespace std;


//==============================================================
// TCP 소켓 기본기능
class CTCPBaseSocket {
protected:
	SOCKET Socket;
public:
	CTCPBaseSocket(SOCKET s);
	virtual ~CTCPBaseSocket();
};


//==============================================================
// TCP 소켓
class CTCPSocket : CTCPBaseSocket {
protected:
	string LineBuffer;
	vector<string> LineSeparator;
public:
	// 생성자
	CTCPSocket(SOCKET s=INVALID_SOCKET);

	// 접속
	bool Connect(string host, int port);

	// 버퍼 송수신: 반환값은 주고받은 바이트 수数
	int Send(const char* buffer, int length);
	int Recv(char* buffer, int length);

	// 행의 송수신
	void SetLineSeparator(vector<string> ls);
	void SendLine(string s);
	string RecvLine();
};


//==============================================================
// TCP 서버 소켓
class CTCPServerSocket : CTCPBaseSocket {
public:
	// 생성자
	CTCPServerSocket();

	// 포트 번호 설정, 접속 허용
	bool Bind(int port);
	CTCPSocket* Accept();
};


#endif

