#include "LibUtil.h"
#include <assert.h>

// 소멸자(세이브)
CArchiveSaver::~CArchiveSaver() {
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		delete[] Entry[i].Data;
	}
}

// 항목(엔트리) 추가
void CArchiveSaver::Add(string name) {
	
	// 엔트리파일 열기
	HANDLE h=CreateFile(
		name.c_str(), GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h!=INVALID_HANDLE_VALUE) {

		// 크기 확인
		DWORD size=GetFileSize(h, NULL);
		char* data=new char[size];
		
		// 엔트리파일 로드
		DWORD nbytes;
		ReadFile(h, data, size, &nbytes, NULL);
		
		// 엔트리 등록
		ARCHIVE_ENTRY e;
		e.Name=name;
		e.Size=size;
		e.Data=data;
		Entry.push_back(e);
		
		// 엔트리파일 닫기
		CloseHandle(h);

		// 헤더크기 계산
		DWORD header_size=sizeof(DWORD)*2;
		for (size_t i=0, n=Entry.size(); i<n; i++) {
			header_size+=Entry[i].Name.length()+1;
			header_size+=sizeof(DWORD)*2;
		}
		
		// 오프셋 계산
		DWORD offset=header_size;
		for (size_t i=0, n=Entry.size(); i<n; i++) {
			Entry[i].Offset=offset;
			offset+=Entry[i].Size+1;
		}
	}
}

// 세이브
bool CArchiveSaver::Save(string name) {

	// 작업 영역 확보
	if (Entry.empty()) return false;
	DWORD work_size=Entry.back().Offset+Entry.back().Size+1;
	char* work=new char[work_size];
	char* p=work+sizeof(DWORD);

	// 엔트리 수 쓰기
	*(DWORD*)p=Entry.size();
	p+=sizeof(DWORD);
	
	// 엔트리 쓰기
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		
		// 이름
		strcpy(p, Entry[i].Name.c_str());
		p+=Entry[i].Name.length()+1;
		
		// 크기
		*(DWORD*)p=Entry[i].Size;
		p+=sizeof(DWORD);
		
		// 오프셋
		*(DWORD*)p=Entry[i].Offset;
		p+=sizeof(DWORD);
	}
	
	// 데이터 쓰기
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		
		// 데이터
		memcpy(p, Entry[i].Data, Entry[i].Size);
		p+=Entry[i].Size;
		
		// NULL 문자
		*p++='\0';
	}
	assert(p-work==work_size);

	// 검사합 (check sum)
	DWORD sum=0;
	for (DWORD i=4, n=work_size-3; i<n; i+=4) {
		sum+=*(DWORD*)(work+i);
	}
	*(DWORD*)work=sum;

	// 스크램블
	Rand.Init(Password);
	for (DWORD i=0, n=work_size-3; i<n; i+=4) {
		*(DWORD*)(work+i)^=Rand.Int32();
	}

	// 아카이브 파일 열기
	HANDLE h=CreateFile(
		name.c_str(), GENERIC_WRITE, 0, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h==INVALID_HANDLE_VALUE) return false;
		
	// 쓰기
	DWORD nbytes;
	WriteFile(h, work, work_size, &nbytes, NULL);

	// 아카이브 파일 닫기
	CloseHandle(h);
	
	// 작업 영역의 해방
	delete[] work;
	
	return true;
}

// 생성자 (로드)
CArchiveLoader::CArchiveLoader()
:	Work(NULL), WorkSize(0) 
{}

// 소멸자 (로드)
CArchiveLoader::~CArchiveLoader() {
	if (Work) delete[] Work;
}

// 로드
bool CArchiveLoader::Load(string name) {
	DWORD nbytes;

	// 아카이브 파일 열기
	HANDLE h=CreateFile(
		name.c_str(), GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h==INVALID_HANDLE_VALUE) return false;

	// 크기 확인
	DWORD size=GetFileSize(h, NULL);
	char* data=new char[size];
	
	// 작업 영역의 확보
	if (WorkSize<size) {
		if (Work) delete[] Work;
		Work=new char[size];
		WorkSize=size;
	}
	
	// 아카이브 파일 로드
	ReadFile(h, Work, size, &nbytes, NULL);
	
	// 아카이브 파일 닫기
	CloseHandle(h);

	// 스크램블 해제
	Rand.Init(Password);
	for (DWORD i=0, n=size-3; i<n; i+=4) {
		*(DWORD*)(Work+i)^=Rand.Int32();
	}

	// 검사합
	DWORD sum=0;
	for (DWORD i=4, n=size-3; i<n; i+=4) {
		sum+=*(DWORD*)(Work+i);
	}
	if (sum!=*(DWORD*)Work) return false;

	// 엔트리 지우기
	Entry.clear();
	
	// 엔트리 수 로드
	char* p=Work+sizeof(DWORD);
	DWORD num_entries=*(DWORD*)p;
	p+=sizeof(DWORD);

	// 엔트리 로드
	for (DWORD i=0; i<num_entries; i++) {
		ARCHIVE_ENTRY e;
		
		// 이름
		e.Name=p;
		p+=e.Name.length()+1;
		
		// 크기
		e.Size=*(DWORD*)p;
		p+=sizeof(DWORD);

		// 오프셋
		e.Offset=*(DWORD*)p;
		p+=sizeof(DWORD);

		// 주소
		e.Data=Work+e.Offset;
		
		// 엔트리 등록
		Entry.push_back(e);
	}
	return true;
}

// 엔트리 검색
ARCHIVE_ENTRY* CArchive::Find(string name) {
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		if (name==Entry[i].Name) return &Entry[i];
	}
	return NULL;
}


