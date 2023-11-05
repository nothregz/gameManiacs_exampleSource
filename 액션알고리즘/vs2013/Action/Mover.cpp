#include "Main.h"
#include "Mover.h"

// 생성자
CMover::CMover()
:	CTask(Game->MoverList),
	X(0), Y(0), W(1), H(1), Angle(0), ShearX(0), 
	Type(0), DrawOrder(0), 
	Texture(NULL), Color(COL_BLACK), 
	ReverseX(false)
{}

void* CMover::operator new(size_t t) {
	return operator_new(t, Game->MoverList);
}
void CMover::operator delete(void* p) {
	operator_delete(p, Game->MoverList);
}

// 그리기
void CMover::Draw() {
	if (Texture) {
		float 
			sw=Game->GetGraphics()->GetWidth()/MAX_X,
			sh=Game->GetGraphics()->GetHeight()/MAX_Y,
			c=cosf(D3DX_PI*2*Angle),
			s=sinf(D3DX_PI*2*Angle),
			dx=W*sw*0.5f,
			dx0=c*dx,
			dx1=s*dx,
			dy=H*sh*0.5f,
			dy0=-s*dy,
			dy1=c*dy,
			x=(X+0.5f)*sw,
			y=(Y+0.5f)*sh,
			u=ReverseX?1:0,
			shx=ShearX*sw;
		Texture->Draw(
			x-dx0-dy0+shx, y-dx1-dy1, Color, u, 0,
			x+dx0-dy0+shx, y+dx1-dy1, Color, 1-u, 0,
			x-dx0+dy0-shx, y-dx1+dy1, Color, u, 1,
			x+dx0+dy0-shx, y+dx1+dy1, Color, 1-u, 1
		);
	}
}

