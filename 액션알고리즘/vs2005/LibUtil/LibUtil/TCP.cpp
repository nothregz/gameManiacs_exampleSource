#include "LibUtil.h"


//==============================================================
// Winsock 초기화 및 종료

// 사용중인 소켓
static int SocketCount=0;

// Winsock 초기화
static void InitializeWinsock() {
	if (SocketCount==0) {
		WSADATA data;
		WSAStartup(MAKEWORD(2,0), &data);
	}
	SocketCount++;
}

// Winsock 종료
static void FinalizeWinsock() {
	SocketCount--;
	if (SocketCount==0) {
		WSACleanup();
	}
}


//==============================================================
// 소켓 기본 기능

// 생성자
CTCPBaseSocket::CTCPBaseSocket(SOCKET s)
:	Socket(s)
{
	InitializeWinsock();
	if (Socket==INVALID_SOCKET) {
		Socket=socket(AF_INET, SOCK_STREAM, 0);
	}
}

// 소멸자
CTCPBaseSocket::~CTCPBaseSocket() {
	closesocket(Socket);
	FinalizeWinsock();
}


//==============================================================
// TCP소켓

// 생성자
CTCPSocket::CTCPSocket(SOCKET s)
:	CTCPBaseSocket(s)
{
	LineSeparator.push_back("\r\n");
	LineSeparator.push_back("\r");
	LineSeparator.push_back("\n");
}

// 접속:
// 성공시 true 반환 
bool CTCPSocket::Connect(string host, int port) {

	// 호스트명을 어드레스로 변환
	LPHOSTENT hostent=gethostbyname(host.c_str());
	if (!hostent ||
		hostent->h_addrtype!=AF_INET ||
		hostent->h_length!=4 ||
		!hostent->h_addr_list[0]) return false;

	// 어드레스와 포트 등록
	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_addr.s_addr=*(unsigned long*)(hostent->h_addr_list[0]);
	sa.sin_port=htons(port);

	// 접속 실행
	return connect(Socket, (sockaddr*)&sa, sizeof(sa))==0;
}

// 버퍼 송신
int CTCPSocket::Send(const char* buffer, int length) {
	return send(Socket, buffer, length, 0);
}

// 버퍼 수신
int CTCPSocket::Recv(char* buffer, int length) {
	return recv(Socket, buffer, length, 0);
}

// 행 세퍼레이터 설정
void CTCPSocket::SetLineSeparator(vector<string> ls) {
	LineSeparator.clear();
	for (size_t i=0, n=ls.size(); i<n; i++) {
		LineSeparator.push_back(ls[i]);
	}
}

// 행 송신
void CTCPSocket::SendLine(string s) {
	if (!LineSeparator.empty()) s+=LineSeparator[0];
	Send(s.c_str(), s.length());
}

// 행 수신
string CTCPSocket::RecvLine() {
	char buffer[1000];
	int length=0;
	do {
		buffer[length]='\0';
		LineBuffer+=buffer;
		for (size_t i=0, n=LineSeparator.size(); i<n; i++) {
			int pos;
			if ((pos=Find(LineBuffer, LineSeparator[i]))>=0) {
				string s=LineBuffer.substr(0, pos);
				LineBuffer=LineBuffer.substr(pos+LineSeparator[i].length());
				return s;
			}
		}
	} while ((length=Recv(buffer, sizeof(buffer)-1))>0);
	return "";
}


//==============================================================
// TCP 서버 소켓

// 생성자
CTCPServerSocket::CTCPServerSocket()
:	CTCPBaseSocket(INVALID_SOCKET)
{}

// 포트 번호 설정:
// 성공시 true 반환
bool CTCPServerSocket::Bind(int port) {
	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_port=htons(port);
	return bind(Socket, (sockaddr*)&sa, sizeof(sa))==0;
}

// 접속 수락:
// 정상적으로 접속한 경우에는 CTCPSocket의 새 인스턴스를 반환
CTCPSocket* CTCPServerSocket::Accept() {

	// 접속 대기
	if (listen(Socket, SOMAXCONN)!=0) return NULL;

	// 접속 허용
	SOCKET socket=accept(Socket, NULL, NULL);
	if (socket==INVALID_SOCKET) return NULL;

	// CTCPSocket 생성
	return new CTCPSocket(socket);
}


