#ifndef ARCHIVEH
#define ARCHIVEH

#include "Rand.h"
#include <windows.h>
#include <vector>
#include <string>
using namespace std;

// 아카이브 된 파일의 엔트리
struct ARCHIVE_ENTRY {
	string Name;
	DWORD Size, Offset;
	char* Data;
};

// 공통 부분
class CArchive {
protected:
	DWORD Version;
	string Password;
	vector<ARCHIVE_ENTRY> Entry;
	CRand Rand;
public:
	CArchive() {}
	void SetPassword(string password) { Password=password; }
	size_t Count() { return Entry.size(); }
	ARCHIVE_ENTRY* Find(string name);
	ARCHIVE_ENTRY* Get(size_t index) { return &Entry[index]; }
};

// 아카이브 작성
class CArchiveSaver : public CArchive {
public:
	CArchiveSaver() {}
	~CArchiveSaver();
	void Add(string name);
	bool Save(string name);
};

// 아카이브 로드
class CArchiveLoader : public CArchive {
protected:
	char* Work;
	DWORD WorkSize;
public:
	CArchiveLoader();
	~CArchiveLoader();
	bool Load(string name);
};

#endif
