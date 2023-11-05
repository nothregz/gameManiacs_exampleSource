#ifndef ARCHIVEH
#define ARCHIVEH

#include "Rand.h"
#include <windows.h>
#include <vector>
#include <string>
using namespace std;

// アーカイブされたファイルのエントリ
struct ARCHIVE_ENTRY {
	wstring Name;
	DWORD Size, Offset;
	char* Data;
};

// 共通部分
class CArchive {
protected:
	DWORD Version;
	wstring Password;
	vector<ARCHIVE_ENTRY> Entry;
	CRand Rand;
public:
	CArchive() {}
	void SetPassword(wstring password) { Password=password; }
	size_t Count() { return Entry.size(); }
	ARCHIVE_ENTRY* Find(wstring name);
	ARCHIVE_ENTRY* Get(size_t index) { return &Entry[index]; }
};

// アーカイブの作成
class CArchiveSaver : public CArchive {
public:
	CArchiveSaver() {}
	~CArchiveSaver();
	void Add(wstring name);
	bool Save(wstring name);
};

// アーカイブのロード
class CArchiveLoader : public CArchive {
protected:
	char* Work;
	DWORD WorkSize;
public:
	CArchiveLoader();
	~CArchiveLoader();
	bool Load(wstring name);
};

#endif
