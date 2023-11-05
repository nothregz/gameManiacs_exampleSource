﻿#ifndef UTILH
#define UTILH

#pragma warning(disable:4786)
#include <vector>
#include <string>
using namespace std;


//==============================================================
// 기타
double GetPreciseTime();

//==============================================================
// 파일 관련
string GetExePath();
string ExtractFilePath(string file);
string ExtractFileName(string file);
bool FileExists(string file);
bool RegFileExt(string ext, string inst, string command, bool force);
string GetSpecialFolderPath(int csidl);
string GetDesktopFolderPath();
int GetWebFile(string url, char** data, string app_name);
void OpenURL(string url);

//==============================================================
// 문자열 관련
int Find(string s, string pattern);
int RFind(string s, string pattern);
class EConvertError {};
int ToInt(string s);
string ToStr(int i);
string TrimLeft(string s);
string TrimRight(string s);
string Trim(string s);
string ConvertEscapes(string s);
bool StartsWith(string s, string pattern);
bool EndsWith(string s, string pattern);
string ToUpper(string s);
string ToLower(string s);

string TrimLeft(string s, string pattern);
string TrimRight(string s, string pattern);
string Trim(string s, string pattern);

string GetToken(string s, int index, string separator);
int GetTokenCount(string s, string separator);

string Replace(string s, string from, string to);
string Replace2(string s, int n, string fromto[][2]);
string ToHan123(string s);
string ToHanABC(string s);
float ToFloat(string s);

//==============================================================
// CStrings 클래스: 여러 행의 문자열을 다룬다
class CStrings {
private:
	vector<string> Strings;
public:
	CStrings();
	void Add(string s);
	int GetCount();
	string GetString(int index);
	inline string Get(int index) { return GetString(index); }
	void SetString(int index, string str);
	string GetText();
	void SetText(string text);
	void Clear();
	void Assign(CStrings* source);

	bool LoadFromFile(string file_name);
	bool SaveToFile(string file_name);
	bool LoadFromWeb(string url, string app_name);

	string GetName(int index);
	string GetValue(int index);
	int FindName(string name);
	string GetValue(string name);
	void SetValue(string name, string value);
	void SetValue(string name, int value);
};


//==============================================================
// 문자판정용 서브 루틴
bool IsAlphabet(char c);
bool IsDigit(char c);
bool IsLeadByte(char c);
bool IsSecondByte(string s, int pos);


//==============================================================
// 문자 코드 관련
string JISToSJIS(string s);
string EUCToSJIS(string s);
string ToSJIS(string s);
string FromBase64(string s);
string ToBase64(string s);


#endif

