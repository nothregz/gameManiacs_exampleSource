#include "LibUtil.h"
#include "libpng\\png.h"
extern "C" {
	#include "libjpeg\\jpeglib.h"
}

//==============================================================
// 초기화

// 초기화 (DIB 섹션)
void CDIB::InitDIB()
{
	if (DIB) DeleteObject(DIB);

	// DIB 섹션의 정보를 유지하는 BITMAPINFO 구조체 작성
	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	BITMAPINFOHEADER* bih=&bi.bmiHeader;
	bih->biSize=sizeof(BITMAPINFOHEADER);
	bih->biWidth=W;
	bih->biHeight=-H;
	bih->biPlanes=1;
	bih->biBitCount=32;
	bih->biCompression=BI_RGB;

	// DIB 섹션 작성
	DIB=CreateDIBSection(
		DC, &bi, DIB_RGB_COLORS,
		(void**)&Pixel, NULL, 0);
	SelectObject(DC, DIB);

	// 알파 영역 작성
	if (Alpha) delete [] Alpha;
	Alpha=new BYTE[W*H];
}

// 초기화（영역）:
// 알파에 기반해 영역 작성
void CDIB::InitRgn() {
	if (Rgn) DeleteObject(Rgn);
	Rgn=CreateRectRgn(0, 0, 0, 0);
	HRGN rgn;
	BYTE* a=Alpha;
	int left;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; ) {
			for (; x<W && *a==0; x++, a++) ;
			left=x;
			for (; x<W && *a!=0; x++, a++) ;
			if (x-left>0) {
				rgn=CreateRectRgn(left, y, x, y+1);
				CombineRgn(Rgn, Rgn, rgn, RGN_OR);
				DeleteObject(rgn);
			}
		}
	}
}

// 초기화（영역）:
// 전체를 덮는 영역 작성 
void CDIB::InitRgnSimple() {
	if (Rgn) DeleteObject(Rgn);
	Rgn=CreateRectRgn(0, 0, W, H);
}


//==============================================================
// 생성자, 소멸자

// 생성자의 공통 처리
void CDIB::Construct() {
	HDC hdc=::GetDC(NULL);
	DC=CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
}

// 생성자 (크기)
CDIB::CDIB(int w, int h)
:	DIB(NULL), Alpha(NULL), Rgn(NULL), W(w), H(h)
{
	Construct();
	InitDIB();
	InitRgnSimple();
}

// 생성자 (캡처)
CDIB::CDIB(HWND hwnd)
:	DIB(NULL), Alpha(NULL), Rgn(NULL)
{
	Construct();
	Capture(hwnd);
}

// 생성자 (파일)
CDIB::CDIB(string file_name)
:	DIB(NULL), Alpha(NULL), Rgn(NULL)
{
	Construct();
	LoadFromFile(file_name);
}

// 소멸자
CDIB::~CDIB() {
	if (DC) DeleteDC(DC);
	if (DIB) DeleteObject(DIB);
	if (Alpha) delete[] Alpha;
	if (Rgn) DeleteObject(Rgn);
}


//==============================================================
// 복사

// CDIB←CDIB
void CDIB::Draw(
	int dx, int dy, int dw, int dh, 
	CDIB* dib, int sx, int sy, int sw, int sh
) {
	StretchBlt(DC, dx, dy, dw, dh, dib->DC, sx, sy, sw, sh, SRCCOPY);
}

// CDIB←HBITMAP
void CDIB::Draw(
	int dx, int dy, int dw, int dh, 
	HBITMAP bmp, int sx, int sy, int sw, int sh
) {
	HDC null_dc=::GetDC(NULL);
	HDC bmp_dc=CreateCompatibleDC(null_dc);
	HGDIOBJ obj=SelectObject(bmp_dc, bmp);
	StretchBlt(DC, dx, dy, dw, dh, bmp_dc, sx, sy, sw, sh, SRCCOPY);
	SelectObject(bmp_dc, obj);
	DeleteDC(bmp_dc);
	ReleaseDC(NULL, null_dc);
}

// DC←CDIB
void CDIB::Draw(
	HDC dc, int dx, int dy, int dw, int dh,
	int sx, int sy, int sw, int sh
) {
	StretchBlt(dc, dx, dy, dw, dh, DC, sx, sy, sw, sh, SRCCOPY);
}


//==============================================================
// 클립 기반 복사 (영역을 사용)

// CDIB←CDIB
void CDIB::ClipDraw(
	int dx, int dy, int dw, int dh, CDIB* dib, int sx, int sy
) {
	SelectClipRgn(DC, dib->Rgn);
	OffsetClipRgn(DC, dx-sx, dy-sy);
	StretchBlt(DC, dx, dy, dw, dh, dib->DC, sx, sy, dw, dh, SRCCOPY);
	SelectClipRgn(DC, NULL);
}

// DC←CDIB
void CDIB::ClipDraw(
	HDC dc, int dx, int dy, int dw, int dh, int sx, int sy
) {
	POINT p;
	GetViewportOrgEx(dc, &p);
	SelectClipRgn(dc, Rgn);
	OffsetClipRgn(dc, p.x+dx-sx, p.y+dy-sy);
	StretchBlt(dc, dx, dy, dw, dh, DC, sx, sy, dw, dh, SRCCOPY);
	SelectClipRgn(dc, NULL);
}


//==============================================================
// 화면 캡처
void CDIB::Capture(HWND hwnd)
{
	// 이미지 크기를 윈도 크기에 맞춘다
	if (!hwnd) hwnd=GetDesktopWindow();	
	GetWindowWH(hwnd, &W, &H);
	InitDIB();
	InitRgnSimple();
	
	// 화면을 캡처한다
	HDC hdc_win=::GetDC(NULL);
	BitBlt(DC, 0, 0, W, H, hdc_win, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hdc_win);
}


//==============================================================
// 투명색 설정:
// 지정한 색상과일치하는 픽셀을 투명하게 한다.
// 다른 색상의 픽셀은 불투명하게 한다.
void CDIB::SetClipColor(DWORD xrgb) {
	xrgb&=0x00ffffff;
	DWORD* p=Pixel;
	BYTE* a=Alpha;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; x++, p++, a++) {
			*a=(*p==xrgb?0:255);
		}
	}
	InitRgn();
}


//==============================================================
// 로드
void CDIB::LoadFromFile(string file_name) {
	if (!FileExists(file_name)) return;
	file_name=ToUpper(file_name);	
	if (EndsWith(file_name, ".BMP")) LoadBMPFile(file_name); else
	if (EndsWith(file_name, ".PNG")) LoadPNGFile(file_name);
	if (EndsWith(file_name, ".JPG") || EndsWith(file_name, ".JPEG")) LoadJPEGFile(file_name);
}

// BMP 형식의 로드
void CDIB::LoadBMPFile(string file_name)
{
	// 비트맵 로드 
	HBITMAP hbmp=(HBITMAP)LoadImage(
		NULL, file_name.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// DIB 초기화
	BITMAP bmp;
	GetObject(hbmp, sizeof(bmp), &bmp);
	W=bmp.bmWidth;
	H=bmp.bmHeight;
	InitDIB();
	FillMemory(Alpha, W*H, 255);
	InitRgnSimple();

	// 비트맵에서 DIB로 복사 
	Draw(0, 0, W, H, hbmp, 0, 0, W, H);
	DeleteObject(hbmp);
}

// PNG 형식 로드 
void CDIB::LoadPNGFile(string file_name)
{
	unsigned int sig_read=0;
	int bit_depth, color_type, interlace_type;
	
	// 파일 열기
	FILE* fp=fopen(file_name.c_str(), "rb");
	if (!fp) return;

	// png_struct 초기화
	png_structp png_ptr=png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(fp);
		return;
	}
	png_init_io(png_ptr, fp);

	// png_info 초기화
	png_infop info_ptr=png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return;
	}

	// 이미지 정보 취득
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, 
		(unsigned long*)&W, (unsigned long*)&H, 
		&bit_depth, &color_type, &interlace_type, NULL, NULL);

	// DIB 초기화
	InitDIB();
	
	// 변환 방법 설정:
	// 팔레트ㅡㄹ RGB로.
	if (color_type==PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// 투명색을 알파로.
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// 비트 심도를 8비트로.
	if (bit_depth<8) png_set_packing(png_ptr); else
	if (bit_depth==16) png_set_strip_16(png_ptr);
	
	// 24비트(RGB)를 32비트(RGB0)으로.
	if (color_type==PNG_COLOR_TYPE_RGB)
		png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);

	// RGB를BGR로.
	if (color_type==PNG_COLOR_TYPE_PALETTE ||
		color_type==PNG_COLOR_TYPE_RGB ||
		color_type==PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(png_ptr);

	// 이미지 로드
	png_bytep *row_pointers=new png_bytep[H];
	for (int row=0; row<H; row++) row_pointers[row]=(png_bytep)(Pixel+row*W);
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, info_ptr);
	delete [] row_pointers;

	// 뒤처리
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	// 알파 꺼내기
	DWORD* p=Pixel;
	BYTE* a=Alpha;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; x++, p++, a++) {
			*a=(BYTE)(*p>>24);
			*p&=0x00ffffff;
		}
	}

	// 영역 초기화
	InitRgn();
}

// JPEG 형식 로드
static void JpegErrorHandler(j_common_ptr) {}

void CDIB::LoadJPEGFile(string file_name)
{
	// 에러 핸들러 설정
	jpeg_decompress_struct jd;
	jpeg_error_mgr jem;
	jd.err=jpeg_std_error(&jem);
	jem.error_exit=JpegErrorHandler;

	// 파일 열기
	FILE* fp=fopen(file_name.c_str(), "rb");
	if (!fp) return;

	// 디코딩 이전 처리
	jpeg_create_decompress(&jd);
	jpeg_stdio_src(&jd, fp);
	jpeg_read_header(&jd, TRUE);
	jpeg_start_decompress(&jd);

	// DIB 초기화
	W=jd.image_width;
	H=jd.image_height;
	InitDIB();
	
	// 디코딩
	for (int row=0; row<H; row++) {
		JSAMPROW jsr=(JSAMPROW)(Pixel+row*W);
		jpeg_read_scanlines(&jd, &jsr, 1);

		// 24비트에서 32비트로 변환
		char *p=(char*)(Pixel+row*W)+3*(W-1), *q=(char*)(Pixel+row*W)+4*(W-1);
		do {
			q[0]=p[0];
			q[1]=p[1];
			q[2]=p[2];
			q[3]=0;
			p-=3;
			q-=4;
		} while (p!=q);
	}

	// 디코딩 이후 처리
	jpeg_finish_decompress(&jd);
	jpeg_destroy_decompress(&jd);

	// 알파 초기화
	BYTE* a=Alpha;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; x++, a++) *a=1;
	}

	// 영역 초기화
	InitRgn();
}
