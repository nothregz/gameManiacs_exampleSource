#include "LibUtil.h"
#include <fstream>
#include <strstream>
#include <windows.h>
#include <assert.h>
#include <mbstring.h>
#include <shlobj.h>
#include <wininet.h>

double GetPreciseTime() {
	LARGE_INTEGER freq, count;
	if (QueryPerformanceFrequency(&freq) && 
		QueryPerformanceCounter(&count)) {
		return (double)count.QuadPart/freq.QuadPart;
	} else {
		return (double)GetTickCount()/1000;
	}
}

void OpenURL(string url) {
	ShellExecute(NULL, NULL, url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

string GetSpecialFolderPath(int csidl) {
	char buf[MAX_PATH];
	SHGetSpecialFolderPath(NULL, buf, csidl, false);
	return string(buf);
}

string GetDesktopFolderPath() {
	return GetSpecialFolderPath(CSIDL_DESKTOP);
}


// 확장자 등록
bool RegFileExt(string ext, string inst, string command, bool force) {
	HKEY key;
	DWORD disp;
	string cls=ext+"_auto_file";
	
	// 확장자 등록
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, (string(".")+ext).c_str(), NULL, "", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	if (!force && disp==REG_OPENED_EXISTING_KEY) {
		RegCloseKey(key);
		return false;
	}
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)cls.c_str(), cls.length());
	RegCloseKey(key);

	// 설명 등록
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, cls.c_str(), NULL, "", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)inst.c_str(), inst.length());
	RegCloseKey(key);

	// 명령 등록
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, (cls+"\\shell\\open\\command").c_str(), NULL, "", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	string cmd=string("\"")+command+"\" \"%1\"";
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)cmd.c_str(), cmd.length());
	RegCloseKey(key);

	// 아이콘 등록
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, (cls+"\\DefaultIcon").c_str(), NULL, "", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	string icon=command+",0";
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)icon.c_str(), icon.length());
	RegCloseKey(key);

	return true;
}


string TrimLeft(string s, string pattern) {
	int pl=pattern.length();
	while (StartsWith(s, pattern)) s=s.substr(pl);
	return s;
}

string TrimRight(string s, string pattern) {
	int sl=s.length(), pl=pattern.length();
	while (EndsWith(s, pattern)) s=s.substr(sl-pl);
	return s;
}

string Trim(string s, string pattern) {
	s=TrimLeft(s, pattern);
	s=TrimRight(s, pattern);
	return s;
}

int Find(string s, string pattern) {
	for (int i=0, n=s.length(), pl=pattern.length(); i<n; i++) {
		if (s.compare(i, pl, pattern)==0) return i;
		if (IsLeadByte(s[i])) i++;
	}
	return string::npos;
}

int RFind(string s, string pattern) {
	for (int i=s.length()-1, pl=pattern.length(); i>=0; i--) {
		if (s.compare(i, pl, pattern)==0 && 
			!IsSecondByte(s, i)) return i;
	}
	return string::npos;
}

string GetToken(string s, int index, string separator) {
	int sl=separator.length();
	for (int i=0; i<index; i++) {
		int pos=Find(s, separator);
		if (pos==string::npos) return "";
		s=s.substr(pos+sl);
	}
	return s.substr(0, Find(s, separator));
}

int GetTokenCount(string s, string separator) {
	int sl=separator.length(), i;
	for (i=1; ; i++) {
		int pos=Find(s, separator);
		if (pos==string::npos) break;
		s=s.substr(pos+sl);
	}
	return i;
}

static string ToHanSub(string s, const char* sa, const char* sz, char a) {
	unsigned int
		ca=(unsigned int)((unsigned char)sa[0])<<8|(unsigned char)sa[1],
		cz=(unsigned int)((unsigned char)sz[0])<<8|(unsigned char)sz[1];
	string t;
	while (!s.empty()) {
		if (IsLeadByte(s[0]) && s.length()>=2) {
			unsigned int 
				c=(unsigned int)((unsigned char)s[0])<<8|(unsigned char)s[1];
			if (ca<=c && c<=cz) {
				t+=(char)(a+(c-ca));
			} else {
				t+=s[0];
				t+=s[1];
			}
			s=s.substr(2);
		} else {
			t+=s[0];
			s=s.substr(1);
		}
	}
	return t;	
}

string ToHan123(string s) {
	return ToHanSub(s, "0", "9", '0');
}

string ToHanABC(string s) {
	string t=s;
	t=ToHanSub(t, "a", "z", 'a');
	t=ToHanSub(t, "A", "Z", 'A');
	return t;
}

string Replace(string s, string from, string to) {
	if (Find(s, from)==string::npos) return s;
	string t;
	int fl=from.length();
	while (!s.empty()) {
		if (StartsWith(s, from)) {
			t+=to;
			s=s.substr(fl);
		} else
		if (IsLeadByte(s[0])) {
			t+=s[0];
			t+=s[1];
			s=s.substr(2);
		} else {
			t+=s[0];
			s=s.substr(1);
		}
	}
	return t;
}

string Replace2(string s, int n, string fromto[][2]) {
	int i;
	for (i=0; i<n; i++) {
		if (Find(s, fromto[i][0])!=string::npos) break;
	}
	if (i==n) return s;
	string t;
	while (!s.empty()) {
		for (i=0; i<n; i++) {
			if (StartsWith(s, fromto[i][0])) {
				t+=fromto[i][1];
				s=s.substr(fromto[i][0].length());
				break;
			}
		}
		if (i==n) {
			if (IsLeadByte(s[0])) {
				t+=s[0];
				t+=s[1];
				s=s.substr(2);
			} else {
				t+=s[0];
				s=s.substr(1);
			}
		}
	}
	return t;
}


float ToFloat(string s) {
	istrstream iss(s.c_str());
	float f=0;
	iss>>f;
	return f;
}

int ToInt(string s) {
	istrstream iss(s.c_str());
	int i=0;
	iss>>i;
	return i;
}




//==============================================================
// 파일 관련


// 실행 파일의 경로 반환 (끝에 \ 붙음)
string GetExePath() {
	char buf[MAX_PATH+1];
	GetModuleFileName(NULL, buf, MAX_PATH);
	return ExtractFilePath(buf);
}


// '경로 + 파일명'에서 경로 추출
string ExtractFilePath(string file) {
	unsigned long pos=RFind(file, "\\");
	if (pos==string::npos) return "";
	return file.substr(0, pos+1);
}


// '경로 + 파일명'에서 파일명 추출
string ExtractFileName(string file) {
	unsigned long pos=RFind(file, "\\");
	if (pos==string::npos) return file;
	return file.substr(pos+1);
}

// 파일이 존재하면 true 반환
bool FileExists(string file) {
	WIN32_FIND_DATA fd;
	return FindFirstFile(file.c_str(), &fd)!=INVALID_HANDLE_VALUE;
}

// Web 상의 파일을 취득
// url은 URL 문자열, data는 취득한 데이터
// 반환값은 데이터의 바이트 수
int GetWebFile(string url, char** data, string app_name) {
	HINTERNET inet, file;
	DWORD data_size=0, read_size;
	char* data_buf=NULL;

	// Web에 연결
	inet=InternetOpen(
		app_name.c_str(), INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	if (inet) {

		// URL에 지정된 파일 열기
		file=InternetOpenUrl(
			inet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if (file) {

			// 읽기 가능한 파일의 크기 확인
			if (InternetQueryDataAvailable(
				file, &read_size, 0, 0)) {

				// 파일 로드
				do {
					
					// 읽기 전용 버퍼를 확장
					data_buf=(char*)realloc(data_buf, data_size+read_size+1);

					// 파일의 일부를 로드
					InternetReadFile(
						file, data_buf+data_size, read_size, &read_size);

					// 읽어들인 크기를 가산
					data_size+=read_size;

				} while (read_size>0);
				data_buf[data_size]='\0';

			}

			// 파일 닫기
			InternetCloseHandle(file);
		}
		
		// 연결 닫기
		InternetCloseHandle(inet);
	}

	*data=data_buf;
	return (int)data_size;
}


//==============================================================
// 문자열 관련

// 대문자로 변환
string ToUpper(string s) {
	string out=s;
	for (int i=0, n=out.size(); i<n; i++) {
		if (IsLeadByte(out[i])) { i++; continue; }
		out[i]=toupper(out[i]);
	}
	return out;
}


// 소문자로 변환
string ToLower(string s) {
	string out=s;
	for (int i=0, n=out.size(); i<n; i++) {
		if (IsLeadByte(out[i])) { i++; continue; }
		out[i]=tolower(out[i]);
	}
	return out;
}

// 문자열의 선두가 지정한 문자열인지 여부를 조사
bool StartsWith(string s, string pattern) {
	return s.compare(0, pattern.length(), pattern)==0;
}


// 문자열의 긑부분이 지정한 문자열인지 여부를 조사
bool EndsWith(string s, string pattern) {
	int len=pattern.length();
	int pos=s.length()-len;
	if (pos<0 || s.compare(pos, len, pattern)!=0) return false;
	return !IsSecondByte(s, pos);
}


// 문자열의 지정한 위치가
// 2바이트 문자의 두 번째 문자인지 여부를 조사
bool IsSecondByte(string s, int pos) {
	if ((unsigned long)pos>=s.length()) return false;
	int i;
	for (i=0; i<pos; i++) if (IsLeadByte(s[i])) i++;
	assert(i>=pos);
	return i>pos;
}


// 정수를 문자열로 변환
string ToStr(int i) {
	char s[20];
	_itoa_s(i, s, sizeof(s), 10);
	return string(s);
}


// 문자열에서 이스케이프 시퀀스를 변환:
// \n은 줄바꿈, \t는 탭으로 변환.
// 기타 「\文字」는 「文字」를 그대로 출력한다.
string ConvertEscapes(string s) {
	string ret;
	int i, n;
	for (i=0, n=s.length(); i<n; i++) {
		if (IsLeadByte(s[i]) && i+1<n) {
			ret+=s[i]; ret+=s[i+1];
			i++;
		} else
		if (s[i]=='\\' && i+1<n) {
			switch (s[i+1]) {
				case 'n': ret+='\n'; break;
				case 't': ret+='\t'; break;
				case '\n': break;
				default: ret+=s[i+1]; break;
			}
			i++;
		} else {
			ret+=s[i];
		}
	}
	return ret;
}


// 문자열의 왼쪽에 있는 공백, 탭, 줄바꿈을 제거 
string TrimLeft(string s) {
	int i, n;
	for (i=0, n=s.length(); i<n ; i++)
		if (s[i]!=' ' && s[i]!='\t' && 
			s[i]!='\n' && s[i]!='\r') break;
	return s.substr(i, n-i);
}


// 문자열의 오른쪽에 있는 공백, 탭, 줄바꿈을 제거
string TrimRight(string s) {
	int i;
	for (i=s.length()-1; i>=0; i--)
		if (s[i]!=' ' && s[i]!='\t' &&
			s[i]!='\n' && s[i]!='\r') break;
	return s.substr(0, i+1);
}


// 문자열의 양쪽에 있는 공백, 탭, 줄바꿈을 제거
string Trim(string s) {
	return TrimRight(TrimLeft(s));
}




//==============================================================
// 문자판정용 서브루틴


// c가 알파벳이라면 '참'을 반환
bool IsAlphabet(char c) {
	return 'a'<=c && c<='z' || 'A'<=c && c<='Z';
}


// c가 숫자라면 '참'을 반환
bool IsDigit(char c) {
	return '0'<=c && c<='9';
}


// c가 2바이트 문자의 첫 글자라면 '참'을 반환 
bool IsLeadByte(char c) {
	int i=(unsigned char)c;
	return (0x81<=i && i<=0x9f) || (0xe0<=i && i<=0xfc);
}




//==============================================================
// CStrings 클래스: 여러 줄의 문자열을 다룸


// 생성자
CStrings::CStrings() {
	Strings.clear();
}


// 끝에 문자열 추가
void CStrings::Add(string s) {
	Strings.push_back(s);
}


// 모든 문자열을 삭제 
void CStrings::Clear() {
	Strings.clear();
}


// 복사
void CStrings::Assign(CStrings* source) {
	Strings.assign(source->Strings.begin(), source->Strings.end());
}


// 행 수를 반환
int CStrings::GetCount() {
	return Strings.size();
}


// 지정한 행을 반환
string CStrings::GetString(int index) {
	if (index<0 || (int)Strings.size()<=index) return "";
	return Strings[index];
}


// 지정한 행에 문자열을 설정
void CStrings::SetString(int index, string str) {
	if (index<0 || (int)Strings.size()<=index) return;
	Strings[index]=str;
}


// 여러 줄의 문자열을 줄바꿈을 끼워넣어 연결하고, 하나의 문자열로서 반환 
string CStrings::GetText() {
	string s;
	int i, n;
	for (i=0, n=Strings.size(); i<n; i++) {
		s+=Strings[i];
		s+='\n';
	}
	return s;
}


// 여러 줄의 문자열을 줄바꿈으로 끊어, 여러 개의 문자열로 만든다
void CStrings::SetText(string text) {
	Clear();
	int pos;
	while ((pos=Find(text, "\n"))!=string::npos) {
		if (pos<1 || text[pos-1]!='\r') {
			Strings.push_back(text.substr(0, pos));
		} else {
			Strings.push_back(text.substr(0, pos-1));
		}
		text=text.substr(pos+1);
	}
	Strings.push_back(text);
}

// 파일에서 여러 줄의 문자열을 로드
bool CStrings::LoadFromFile(string file_name) {
	ifstream fin;
	static const int BUFSIZE=1024;
	static char buf[BUFSIZE];
	Clear();
	fin.open(file_name.c_str());
	if (fin.fail()) return false;
	for (fin.getline(buf, BUFSIZE); !fin.fail(); fin.getline(buf, BUFSIZE))
		Strings.push_back(buf);
	fin.close();
	return true;
}

// 파일에서 여러 줄의 문자열을 내보냄
bool CStrings::SaveToFile(string file_name) {
	ofstream fout;
	fout.open(file_name.c_str());
	if (fout.fail()) return false;
	fout<<GetText();
	fout.close();
	return true;
}

// Web 상의 파일에서 여러 줄의 문자열을 로드 
bool CStrings::LoadFromWeb(string url, string app_name) {
	char* buffer;
	if (!GetWebFile(url, &buffer, app_name)) return false;
	SetText(ToSJIS(buffer));
	free(buffer);
	return true;
}


//==============================================================
static const char* copyright=
	"Copyright(C) 2004 마츠우라 켄이치로/ 츠카사 유키";


//==============================================================
// CStrings 클래스에서,
// 문자열을 '이름 = 값' 형식으로 해석하는 함수군

// 지정한 행의 이름 부분을 반환
string CStrings::GetName(int index) {
	if (index<0 || (int)Strings.size()<=index) return "";
	string s=Strings[index];
	int pos=Find(s, "=");
	if (pos<0) return "";
	return s.substr(0, pos);
}

// 지정한 행의 값 부분을 반환
string CStrings::GetValue(int index) {
	if (index<0 || (int)Strings.size()<=index) return "";
	string s=Strings[index];
	int pos=Find(s, "=");
	if (pos<0) return "";
	return s.substr(pos+1, s.length()-pos-1);
}

// 지정한 이름을 가지는 최초의 행 번호를 반환 
int CStrings::FindName(string name) {
	int i, n;
	for (i=0, n=Strings.size(); i<n; i++) {
		if (GetName(i)==name) return i;
	}
	return -1;
}

// 지정한 이름을 가지는 첫번째 행을 찾아서 값을 변환 :
// 이름이 미등록이면 빈 문자열을 반환
string CStrings::GetValue(string name) {
	int i=FindName(name);
	if (i<0) return "";
	return GetValue(i);
}

// 지정한 이름을 가지는 첫번째 행을 찾고 문자열값을 설정
void CStrings::SetValue(string name, string value) {
	int i=FindName(name);
	if (i>=0) Strings[i]=name+"="+value;
	else Strings.push_back(name+"="+value);
}

// 지정한 이름을 가지는 첫번째 행을 찾고, 정수값을 설정
void CStrings::SetValue(string name, int value) {
	char buf[20];
	_itoa_s(value, buf, sizeof(buf), 10);
	SetValue(name, buf);
}


string Encode(string str) {
	string s=str;
	for (int i=0, n=s.length(), rnd=0xaa; i<n; i++) {
		s[i]=s[i]^(char)rnd;
		rnd=214013*rnd+2531011;
	}
	return ToBase64(s);
}

string Decode(string str) {
	string s=FromBase64(str);
	for (int i=0, n=s.length(), rnd=0xaa; i<n; i++) {
		s[i]=s[i]^(char)rnd;
		rnd=214013*rnd+2531011;
	}
	return s;
}


//==============================================================
// 문자 코드 관련


// 문자열 코드를 JIS에서 Shift-JIS로 변환 :
// 입력문자열을 JIS에서 Shift-JIS로 변환한 문자열을 반환한다.
string JISToSJIS(string s) {
	string ret;
	unsigned char c0, c1;
	bool kanji=false;

	for (int i=0, n=(int)s.length(); i<n; i++) {
	
		// 이스케이프 시퀀스 판별
		if (s[i]==0x1b) {
			i++;
			if (i>=n) break;
			c0=s[i++];
			if (c0==0x24) kanji=true; else
			if (c0==0x28) kanji=false;
			continue;
		}

		// 한자
		if (kanji) {
			c0=s[i++];
			if (i>=n) break;
			c1=s[i];
			ret+=(char)(((c0+1)>>1)+(c0<0x5f ? 0x70 : 0xb0));
			ret+=(char)(c1+(c0%2 ? (c1>0x5f ? 0x20 : 0x1f) : 0x7e));
		} else

		// ASCII 문자
		{
			ret+=s[i];
		}
	}
	return ret;
}


// 문자열 코드를 EUC에서 Shift-JIS로 변환: 
// 입력문자열을 EUC에서 Shift-JIS로 변환한 문자열을 반환한다. 
string EUCToSJIS(string s) {
	string ret;
	int i;
	unsigned char c0, c1;
	for (i=0; i<(int)s.length(); i++) {

		// 1바이트 문자는 그대로 복사한다 
		c0=s[i];
		if (c0<=0xa0 || c0==0xff || 
			(unsigned long)i==s.length()-1) {
			ret+=c0;
			continue;
		} 
		
		// 2바이트 문자의 두번째 바이트의 변환 
		c1=s[i+1];
		if (c0%2) {
			c1-=0x61;
			if (c1>0x7e) c1++;
		} else {
			c1-=0x02;
		}

		// 2바이트 문자의 첫번째 바이트의 변환 
		if (c0>=0xdf) {
			c0=(c0+1)/2+0x70;
		} else {
			c0=(c0+1)/2+0x30;
		}
		ret+=c0;
		ret+=c1;
		i++;
	}
	return ret;
}


// 문자열 코드를 Shift-JIS로 변환:
// 입력문자열의 문자코드를 판별하고, EUC의 경우 Shift-JIS로 변환한다. 
// 처음부터 Shift-JIS의 경우에는 아무것도 하지 않는다. 
string ToSJIS(string s) {
	int i;
	unsigned char c;
	for (i=0; i<(int)s.length(); i++) {

		// Shift-JIS밖에 없는 문자를 발견하면 
		// 변환하지 않고 반환한다.
		c=s[i];
		if (0x81<=c && c<=0x9f) return s;
	}
	
	// Shift-JIS밖에 없는 문자가 발견되지 않았기 때문에 
	// EUC라고 상정하고 변환한다.
	return EUCToSJIS(s);
}


// BASE64 형식의 문자열을 디코딩 
string FromBase64(string s) {
	string ret;
	unsigned char c[4];
	for (int i=0, n=s.length(); i<n; i+=4) {
		int j;
		
		// 'A'~'Z', 'a'~'z', '0'~'9', '+', '/'을
		// 0~63의 숫자로 변환한다. 
		for (j=0; j<4 && i+j<n; j++) {
			c[j]=s[i+j];
			if ('A'<=c[j] && c[j]<='Z') c[j]-='A'; else
			if ('a'<=c[j] && c[j]<='z') c[j]-='a'-26; else
			if ('0'<=c[j] && c[j]<='9') c[j]-='0'-52; else
			if (c[j]=='+') c[j]=62; else
			if (c[j]=='/') c[j]=63; else break;
		}
		
		// 6비트값 4개를 8비트값 3개로 변환한다.
		if (j>1) ret+=(c[0]<<2 | c[1]>>4);
		if (j>2) ret+=(c[1]<<4 | c[2]>>2);
		if (j>3) ret+=(c[2]<<6 | c[3]);
	}
	return ret;
}


// 문자열을 BASE64 형식으로 인코딩 
string ToBase64(string s) {
	string ret;
	unsigned char c[3], d[4];
	for (int i=0, n=s.length(); i<n; i+=3) {
		int j;

		// 3문자를 추출
		for (j=0; j<3; j++) c[j]=i+j<n?s[i+j]:0;
		
		// 8비트값 3개를, 6비트값 4개로 변환한다.
		d[0]=(c[0]>>2)&63;
		d[1]=(c[0]<<4|c[1]>>4)&63;
		d[2]=(c[1]<<2|c[2]>>6)&63;
		d[3]=c[2]&63;

		// 0~63의 수치를, 
		// 'A'~'Z', 'a'~'z', '0'~'9','+', '/'로 변환한다.
		for (j=0; j<4; j++) {
			if (d[j]<=25) d[j]+='A'; else
			if (26<=d[j] && d[j]<=51) d[j]+='a'-26; else
			if (52<=d[j] && d[j]<=61) d[j]+='0'-52; else
			if (d[j]==62) d[j]='+'; else
			if (d[j]==63) d[j]='/'; else d[j]='A';
		}

		// 결과를 문자열에 추가한다:
		// 문자수가 4의 배수가 되도록,
		// 필요에 따라 끝에 '='를 추가한다.
		ret+=d[0];
		ret+=d[1];
		ret+=i+1<n?d[2]:'=';
		ret+=i+2<n?d[3]:'=';
	}
	return ret;
}



