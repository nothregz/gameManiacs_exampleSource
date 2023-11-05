#include "LibUtil.h"
#include <fstream>
#include <sstream>
#include <windows.h>
#include <assert.h>
#include <mbstring.h>
#include <shlobj.h>
#include <wininet.h>

//==================================================================
// �����R�[�h�ϊ�

wstring MultiToWide(string s, int encoding) {
	int wsize=MultiByteToWideChar(encoding, 0, s.c_str(), s.length(), NULL, 0);
	if (wsize>0) {
		wchar_t* ws=new wchar_t[wsize];
		if (MultiByteToWideChar(encoding, 0, s.c_str(), s.length(), ws, wsize)==wsize) {
			return wstring(ws);
		}
	}
	return L"";
}

string WideToMulti(wstring ws, int encoding) {
	int size=WideCharToMultiByte(encoding, 0, ws.c_str(), -1, NULL, 0, NULL, NULL);
	if (size>0) {
		char* s=new char[size];
		if (WideCharToMultiByte(encoding, 0, ws.c_str(), -1, s, size, NULL, NULL)==size) {
			return string(s);
		}
	}
	return "";
}



double GetPreciseTime() {
	LARGE_INTEGER freq, count;
	if (QueryPerformanceFrequency(&freq) && 
		QueryPerformanceCounter(&count)) {
		return (double)count.QuadPart/freq.QuadPart;
	} else {
		return (double)GetTickCount()/1000;
	}
}

void OpenURL(wstring url) {
	ShellExecute(NULL, NULL, url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

wstring GetSpecialFolderPath(int csidl) {
	wchar_t buf[MAX_PATH];
	SHGetSpecialFolderPath(NULL, buf, csidl, false);
	return wstring(buf);
}

wstring GetDesktopFolderPath() {
	return GetSpecialFolderPath(CSIDL_DESKTOP);
}


// �g���q�̓o�^
bool RegFileExt(wstring ext, wstring inst, wstring command, bool force) {
	HKEY key;
	DWORD disp;
	wstring cls=ext+L"_auto_file";
	
	// �g���q�̓o�^
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, (wstring(L".")+ext).c_str(), NULL, L"", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	if (!force && disp==REG_OPENED_EXISTING_KEY) {
		RegCloseKey(key);
		return false;
	}
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)cls.c_str(), cls.length());
	RegCloseKey(key);

	// �����̓o�^
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, cls.c_str(), NULL, L"", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)inst.c_str(), inst.length());
	RegCloseKey(key);

	// �R�}���h�̓o�^
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, (cls+L"\\shell\\open\\command").c_str(), NULL, L"", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	wstring cmd=wstring(L"\"")+command+L"\" \"%1\"";
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)cmd.c_str(), cmd.length());
	RegCloseKey(key);

	// �A�C�R���̓o�^
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, (cls+L"\\DefaultIcon").c_str(), NULL, L"", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	wstring icon=command+L",0";
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)icon.c_str(), icon.length());
	RegCloseKey(key);

	return true;
}


wstring TrimLeft(wstring s, wstring pattern) {
	int pl=pattern.length();
	while (StartsWith(s, pattern)) s=s.substr(pl);
	return s;
}

wstring TrimRight(wstring s, wstring pattern) {
	int sl=s.length(), pl=pattern.length();
	while (EndsWith(s, pattern)) s=s.substr(sl-pl);
	return s;
}

wstring Trim(wstring s, wstring pattern) {
	s=TrimLeft(s, pattern);
	s=TrimRight(s, pattern);
	return s;
}

int Find(wstring s, wstring pattern) {
	for (int i=0, n=s.length(), pl=pattern.length(); i<n; i++) {
		if (s.compare(i, pl, pattern)==0) return i;
	}
	return wstring::npos;
}

int RFind(wstring s, wstring pattern) {
	for (int i=s.length()-1, pl=pattern.length(); i>=0; i--) {
		if (s.compare(i, pl, pattern)==0) return i;
	}
	return wstring::npos;
}

wstring GetToken(wstring s, int index, wstring separator) {
	int sl=separator.length();
	for (int i=0; i<index; i++) {
		int pos=Find(s, separator);
		if (pos==wstring::npos) return L"";
		s=s.substr(pos+sl);
	}
	return s.substr(0, Find(s, separator));
}

int GetTokenCount(wstring s, wstring separator) {
	int sl=separator.length(), i;
	for (i=1; ; i++) {
		int pos=Find(s, separator);
		if (pos==wstring::npos) break;
		s=s.substr(pos+sl);
	}
	return i;
}

wstring ToHan123(wstring s) {
	wstring t;
	for (int i=0, n=s.length(); i<n; i++) {
		if (L'0'<=s[i] && s[i]<=L'9') t+=s[i]+L'�O'; else t+=s[i];
	}
	return t;
}

wstring ToHanABC(wstring s) {
	wstring t;
	for (int i=0, n=s.length(); i<n; i++) {
		if (L'a'<=s[i] && s[i]<=L'z') t+=s[i]+L'��'; 
		if (L'A'<=s[i] && s[i]<=L'Z') t+=s[i]+L'�`'; else t+=s[i];
	}
	return t;
}

wstring Replace(wstring s, wstring from, wstring to) {
	if (Find(s, from)==wstring::npos) return s;
	wstring t;
	int fl=from.length();
	while (!s.empty()) {
		if (StartsWith(s, from)) {
			t+=to;
			s=s.substr(fl);
		} else {
			t+=s[0];
			s=s.substr(1);
		}
	}
	return t;
}

wstring Replace2(wstring s, int n, wstring fromto[][2]) {
	int i;
	for (i=0; i<n; i++) {
		if (Find(s, fromto[i][0])!=wstring::npos) break;
	}
	if (i==n) return s;
	wstring t;
	while (!s.empty()) {
		for (i=0; i<n; i++) {
			if (StartsWith(s, fromto[i][0])) {
				t+=fromto[i][1];
				s=s.substr(fromto[i][0].length());
				break;
			}
		}
		if (i==n) {
			t+=s[0];
			s=s.substr(1);
		}
	}
	return t;
}


float ToFloat(wstring s) {
	wistringstream iss(s.c_str());
	float f=0;
	iss>>f;
	return f;
}

int ToInt(wstring s) {
	wistringstream iss(s.c_str());
	int i=0;
	iss>>i;
	return i;
}




//==============================================================
// �t�@�C���֘A


// ���s�t�@�C���̃p�X��Ԃ��i������\���t���j
wstring GetExePath() {
	wchar_t buf[MAX_PATH+1];
	GetModuleFileName(NULL, buf, MAX_PATH);
	return ExtractFilePath(buf);
}


// �u�p�X�{�t�@�C�����v����p�X�����𔲂��o��
wstring ExtractFilePath(wstring file) {
	unsigned long pos=RFind(file, L"\\");
	if (pos==wstring::npos) return L"";
	return file.substr(0, pos+1);
}


// �u�p�X�{�t�@�C�����v����t�@�C���������𔲂��o��
wstring ExtractFileName(wstring file) {
	unsigned long pos=RFind(file, L"\\");
	if (pos==wstring::npos) return file;
	return file.substr(pos+1);
}

// �t�@�C�������݂���Ȃ�true��Ԃ�
bool FileExists(wstring file) {
	WIN32_FIND_DATA fd;
	return FindFirstFile(file.c_str(), &fd)!=INVALID_HANDLE_VALUE;
}

// Web��̃t�@�C�����擾����
// url��URL�̕�����Cdata�͎擾�����f�[�^
// �߂�l�̓f�[�^�̃o�C�g��
int GetWebFile(wstring url, char** data, wstring app_name) {
	HINTERNET inet, file;
	DWORD data_size=0, read_size;
	char* data_buf=NULL;

	// Web�ɐڑ�����
	inet=InternetOpen(
		app_name.c_str(), INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	if (inet) {

		// URL�Ŏw�肳�ꂽ�t�@�C�����J��
		file=InternetOpenUrl(
			inet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if (file) {

			// �ǂݍ��݉\�ȃt�@�C���̃T�C�Y�𒲂ׂ�
			if (InternetQueryDataAvailable(
				file, &read_size, 0, 0)) {

				// �t�@�C����ǂݍ���
				do {
					
					// �ǂݍ��ݗp�̃o�b�t�@���g������
					data_buf=(char*)realloc(data_buf, data_size+read_size+1);

					// �t�@�C���̈ꕔ��ǂݍ���
					InternetReadFile(
						file, data_buf+data_size, read_size, &read_size);

					// �ǂݍ��񂾃T�C�Y�����Z����
					data_size+=read_size;

				} while (read_size>0);
				data_buf[data_size]='\0';

			}

			// �t�@�C�������
			InternetCloseHandle(file);
		}
		
		// �ڑ������
		InternetCloseHandle(inet);
	}

	*data=data_buf;
	return (int)data_size;
}


//==============================================================
// ������֘A

// �啶���ւ̕ϊ�
wstring ToUpper(wstring s) {
	wstring out=s;
	for (int i=0, n=out.size(); i<n; i++) {
		out[i]=toupper(out[i]);
	}
	return out;
}


// �������ւ̕ϊ�
wstring ToLower(wstring s) {
	wstring out=s;
	for (int i=0, n=out.size(); i<n; i++) {
		out[i]=tolower(out[i]);
	}
	return out;
}

// ������̐擪���w�肵�������񂩂ǂ����𒲂ׂ�
bool StartsWith(wstring s, wstring pattern) {
	return s.compare(0, pattern.length(), pattern)==0;
}


// ������̖������w�肵�������񂩂ǂ����𒲂ׂ�
bool EndsWith(wstring s, wstring pattern) {
	int len=pattern.length();
	int pos=s.length()-len;
	if (pos<0 || s.compare(pos, len, pattern)!=0) return false;
	return true;
}


// �����𕶎���ɕϊ�����
wstring ToStr(int i) {
	wostringstream oss;
	oss<<i;
	return oss.str();
}


// ������̒��̃G�X�P�[�v�V�[�P���X��ϊ�����F
// \n�͉��s�ɁC\t�̓^�u�ɕϊ��B
// ���̑��́u\�����v�́u�����v�����̂܂܏o�͂���B
wstring ConvertEscapes(wstring s) {
	wstring ret;
	int i, n;
	for (i=0, n=s.length(); i<n; i++) {
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


// ������̍����ɂ���󔒁C�^�u�C���s����������
wstring TrimLeft(wstring s) {
	int i, n;
	for (i=0, n=s.length(); i<n ; i++)
		if (s[i]!=' ' && s[i]!='\t' && 
			s[i]!='\n' && s[i]!='\r') break;
	return s.substr(i, n-i);
}


// ������̉E���ɂ���󔒁C�^�u�C���s����������
wstring TrimRight(wstring s) {
	int i;
	for (i=s.length()-1; i>=0; i--)
		if (s[i]!=' ' && s[i]!='\t' &&
			s[i]!='\n' && s[i]!='\r') break;
	return s.substr(0, i+1);
}


// ������̗����ɂ���󔒁C�^�u�C���s����������
wstring Trim(wstring s) {
	return TrimRight(TrimLeft(s));
}




//==============================================================
// ��������p�̃T�u���[�`��


// c���A���t�@�x�b�g�Ȃ�ΐ^��Ԃ�
bool IsAlphabet(char c) {
	return 'a'<=c && c<='z' || 'A'<=c && c<='Z';
}


// c�������Ȃ�ΐ^��Ԃ�
bool IsDigit(char c) {
	return '0'<=c && c<='9';
}



//==============================================================
// CStrings�N���X�F�����s�̕����������


// �R���X�g���N�^
CStrings::CStrings() {
	Strings.clear();
}


// �����ɕ������ǉ�����
void CStrings::Add(wstring s) {
	Strings.push_back(s);
}


// �S�Ă̕��������������
void CStrings::Clear() {
	Strings.clear();
}


// �R�s�[����
void CStrings::Assign(CStrings* source) {
	Strings.assign(source->Strings.begin(), source->Strings.end());
}


// �s����Ԃ�
int CStrings::GetCount() {
	return Strings.size();
}


// �w�肵���s��Ԃ�
wstring CStrings::GetString(int index) {
	if (index<0 || (int)Strings.size()<=index) return L"";
	return Strings[index];
}


// �w�肵���s�ɕ������ݒ肷��
void CStrings::SetString(int index, wstring str) {
	if (index<0 || (int)Strings.size()<=index) return;
	Strings[index]=str;
}


// �����s�̕���������s���͂���ŘA�����C�P��̕�����Ƃ��ĕԂ�
wstring CStrings::GetText() {
	wstring s;
	int i, n;
	for (i=0, n=Strings.size(); i<n; i++) {
		s+=Strings[i];
		s+=L'\n';
	}
	return s;
}


// �����s�̕���������s�Ő؂�C�����̕�����ɂ���
void CStrings::SetText(wstring text) {
	Clear();
	int pos;
	while ((pos=Find(text, L"\n"))!=wstring::npos) {
		if (pos<1 || text[pos-1]!=L'\r') {
			Strings.push_back(text.substr(0, pos));
		} else {
			Strings.push_back(text.substr(0, pos-1));
		}
		text=text.substr(pos+1);
	}
	Strings.push_back(text);
}

// �t�@�C�����畡���s�̕������ǂݍ���
bool CStrings::LoadFromFile(wstring file_name) {
	wifstream fin;
	static const int BUFSIZE=1024;
	static wchar_t buf[BUFSIZE];
	Clear();
	fin.open(file_name.c_str());
	if (fin.fail()) return false;
	for (fin.getline(buf, BUFSIZE); !fin.fail(); fin.getline(buf, BUFSIZE))
		Strings.push_back(buf);
	fin.close();
	return true;
}

// �t�@�C���ɕ����s�̕�����������o��
bool CStrings::SaveToFile(wstring file_name) {
	wofstream fout;
	fout.open(file_name.c_str());
	if (fout.fail()) return false;
	fout<<GetText();
	fout.close();
	return true;
}

// Web��̃t�@�C�����畡���s�̕������ǂݍ���
bool CStrings::LoadFromWeb(wstring url, wstring app_name) {
	char* buffer;
	if (!GetWebFile(url, &buffer, app_name)) return false;
	SetText(MultiToWide(ToSJIS(buffer), 932));
	free(buffer);
	return true;
}


//==============================================================
static const wchar_t* copyright=
	L"Copyright(C) 2004 �Ђ��؂�H�[�i���Y����Y�^�i�䂫�j";


//==============================================================
// CStrings�N���X�̒��ŁC
// ��������u���O=�l�v�`���Ƃ��ĉ��߂���֐��Q

// �w�肵���s�̖��O�̕�����Ԃ�
wstring CStrings::GetName(int index) {
	if (index<0 || (int)Strings.size()<=index) return L"";
	wstring s=Strings[index];
	int pos=Find(s, L"=");
	if (pos<0) return L"";
	return s.substr(0, pos);
}

// �w�肵���s�̒l�̕�����Ԃ�
wstring CStrings::GetValue(int index) {
	if (index<0 || (int)Strings.size()<=index) return L"";
	wstring s=Strings[index];
	int pos=Find(s, L"=");
	if (pos<0) return L"";
	return s.substr(pos+1, s.length()-pos-1);
}

// �w�肵�����O�����ŏ��̍s�ԍ���Ԃ�
int CStrings::FindName(wstring name) {
	int i, n;
	for (i=0, n=Strings.size(); i<n; i++) {
		if (GetName(i)==name) return i;
	}
	return -1;
}

// �w�肵�����O�����ŏ��̍s��T���C�l��Ԃ��F
// ���O�����o�^�Ȃ�΋󕶎����Ԃ�
wstring CStrings::GetValue(wstring name) {
	int i=FindName(name);
	if (i<0) return L"";
	return GetValue(i);
}

// �w�肵�����O�����ŏ��̍s��T���C������l��ݒ肷��
void CStrings::SetValue(wstring name, wstring value) {
	int i=FindName(name);
	if (i>=0) Strings[i]=(name+L"=")+value;
	else Strings.push_back((name+L"=")+value);
}

// �w�肵�����O�����ŏ��̍s��T���C�����l��ݒ肷��
void CStrings::SetValue(wstring name, int value) {
	SetValue(name, ToStr(value));
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
// �����R�[�h�֘A


// ������̃R�[�h��JIS����V�t�gJIS�ɕϊ�����F
// ���͕������JIS����V�t�gJIS�ɕϊ������������Ԃ��B
string JISToSJIS(string s) {
	string ret;
	unsigned char c0, c1;
	bool kanji=false;

	for (int i=0, n=(int)s.length(); i<n; i++) {
	
		// �G�X�P�[�v�V�[�P���X�̔���
		if (s[i]==0x1b) {
			i++;
			if (i>=n) break;
			c0=s[i++];
			if (c0==0x24) kanji=true; else
			if (c0==0x28) kanji=false;
			continue;
		}

		// ����
		if (kanji) {
			c0=s[i++];
			if (i>=n) break;
			c1=s[i];
			ret+=(char)(((c0+1)>>1)+(c0<0x5f ? 0x70 : 0xb0));
			ret+=(char)(c1+(c0%2 ? (c1>0x5f ? 0x20 : 0x1f) : 0x7e));
		} else

		// ASCII����
		{
			ret+=s[i];
		}
	}
	return ret;
}


// ������̃R�[�h��EUC����V�t�gJIS�ɕϊ�����F
// ���͕������EUC����V�t�gJIS�ɕϊ������������Ԃ��B
string EUCToSJIS(string s) {
	string ret;
	int i;
	unsigned char c0, c1;
	for (i=0; i<(int)s.length(); i++) {

		// 1�o�C�g�����͂��̂܂܃R�s�[����
		c0=s[i];
		if (c0<=0xa0 || c0==0xff || 
			(unsigned long)i==s.length()-1) {
			ret+=c0;
			continue;
		} 
		
		// 2�o�C�g������2�o�C�g�ڂ̕ϊ�
		c1=s[i+1];
		if (c0%2) {
			c1-=0x61;
			if (c1>0x7e) c1++;
		} else {
			c1-=0x02;
		}

		// 2�o�C�g������1�o�C�g�ڂ̕ϊ�
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


// ������̃R�[�h���V�t�gJIS�ɕϊ�����F
// ���͕�����̕����R�[�h�𔻕ʂ��āCEUC�̏ꍇ�ɂ̓V�t�gJIS�ɕϊ�����B
// ���X�V�t�gJIS�̏ꍇ�ɂ͉������Ȃ��B
string ToSJIS(string s) {
	int i;
	unsigned char c;
	for (i=0; i<(int)s.length(); i++) {

		// �V�t�gJIS�ɂ����Ȃ������𔭌�������C
		// �ϊ������ɖ߂�
		c=s[i];
		if (0x81<=c && c<=0x9f) return s;
	}
	
	// �V�t�gJIS�ɂ����Ȃ������������ł��Ȃ������̂ŁC
	// EUC���Ƒz�肵�ĕϊ�����
	return EUCToSJIS(s);
}


// BASE64�`���̕�������f�R�[�h����
string FromBase64(string s) {
	string ret;
	unsigned char c[4];
	for (int i=0, n=s.length(); i<n; i+=4) {
		int j;
		
		// 'A'�`'Z'�C'a'�`'z'�C'0'�`'9'�C'+'�C'/�f���C
		// 0�`63�̐��l�ɕϊ�����B
		for (j=0; j<4 && i+j<n; j++) {
			c[j]=s[i+j];
			if ('A'<=c[j] && c[j]<='Z') c[j]-='A'; else
			if ('a'<=c[j] && c[j]<='z') c[j]-='a'-26; else
			if ('0'<=c[j] && c[j]<='9') c[j]-='0'-52; else
			if (c[j]=='+') c[j]=62; else
			if (c[j]=='/') c[j]=63; else break;
		}
		
		// 6�r�b�g�l4���C8�r�b�g�l3�ɕϊ�����
		if (j>1) ret+=(c[0]<<2 | c[1]>>4);
		if (j>2) ret+=(c[1]<<4 | c[2]>>2);
		if (j>3) ret+=(c[2]<<6 | c[3]);
	}
	return ret;
}


// �������BASE64�`���ɃG���R�[�h����
string ToBase64(string s) {
	string ret;
	unsigned char c[3], d[4];
	for (int i=0, n=s.length(); i<n; i+=3) {
		int j;

		// 3���������o��
		for (j=0; j<3; j++) c[j]=i+j<n?s[i+j]:0;
		
		// 8�r�b�g�l3���C6�r�b�g�l4�ɕϊ�����
		d[0]=(c[0]>>2)&63;
		d[1]=(c[0]<<4|c[1]>>4)&63;
		d[2]=(c[1]<<2|c[2]>>6)&63;
		d[3]=c[2]&63;

		// 0�`63�̐��l���C
		// 'A'�`'Z'�C'a'�`'z'�C'0'�`'9'�C'+'�C'/�f�ɕϊ�����
		for (j=0; j<4; j++) {
			if (d[j]<=25) d[j]+='A'; else
			if (26<=d[j] && d[j]<=51) d[j]+='a'-26; else
			if (52<=d[j] && d[j]<=61) d[j]+='0'-52; else
			if (d[j]==62) d[j]='+'; else
			if (d[j]==63) d[j]='/'; else d[j]='A';
		}

		// ���ʂ𕶎���ɒǉ�����F
		// ��������4�̔{���ɂȂ�悤�ɁC
		// �K�v�ɉ����Ė�����'='��������
		ret+=d[0];
		ret+=d[1];
		ret+=i+1<n?d[2]:'=';
		ret+=i+2<n?d[3]:'=';
	}
	return ret;
}



