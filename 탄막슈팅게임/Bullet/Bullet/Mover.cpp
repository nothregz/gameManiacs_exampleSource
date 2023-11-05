#include "Mover.h"

//==============================================================
// 移動物体

CMover::CMover(CTaskList* list, int shape_id, float x, float y, float angle)
:	CTask(list), 
	Shape(&ShapeList[shape_id]), 
	X(x), Y(y), Angle(angle), 
	Scale(1), Alpha(1),
	Alive(true), Color(-1)
{}

void CMover::Move() {
}

void CMover::DrawSub(CTexture* texture, float size, float fade) {
	if (texture) {
		float 
			sw=Graphics->GetHeight()*MaxX/MaxY*0.5f,
			sh=Graphics->GetHeight()/MaxY*0.5f,
			c=cosf(D3DX_PI*2*Angle),
			s=sinf(D3DX_PI*2*Angle),
			dx=size*Scale*sw,
			dx0=c*dx,
			dx1=s*dx,
			dy=size*Scale*sh,
			dy0=-s*dy,
			dy1=c*dy,
			x=(X+1)*sw,
			y=(Y+1)*sh;
		D3DCOLOR color=D3DXCOLOR(1, 1, 1, Alpha*fade);
		texture->Draw(
			x-dx0-dy0, y-dx1-dy1, color, 0, 0,
			x+dx0-dy0, y+dx1-dy1, color, 1, 0,
			x-dx0+dy0, y-dx1+dy1, color, 0, 1,
			x+dx0+dy0, y+dx1+dy1, color, 1, 1
		);
	}
}

void CMover::Draw() {
	int i=ShowColor?1:0;
	if (ShowHit) {
		DrawSub(Shape->Texture[i], Shape->Size, 0.25f);
		DrawSub(ShapeList[ShHit].Texture[i], Shape->Hit, 1);
	} else {
		DrawSub(Shape->Texture[i], Shape->Size, 1);
		if (ShowColor) {
			LPDIRECT3DDEVICE9 device=Graphics->GetDevice();
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			DrawSub(Shape->Texture[i], Shape->Size*1.1f, 0.3f);
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
	}
}

bool CMover::IsHit(CMover* mover) {
	float 
		dx=mover->X-X, 
		dy=mover->Y-Y,
		hit=mover->Shape->Hit+Shape->Hit;
	return dx*dx+dy*dy<hit*hit;
}

bool CMover::IsHit(CTaskList* list) {
	for (CTaskIter i(list); i.HasNext(); ) {
		CMover* mover=static_cast<CMover*>(i.Next());
		if (IsHit(mover)) return true;
	}
	return false;
}

void CMover::SetShape(int shape_id) {
	Shape=&ShapeList[shape_id];
}

//==============================================================
// 自機

CMyShip::CMyShip(int shape_id)
:	CMover(MyShipList, shape_id, 0, MaxY-ShapeList[shape_id].Size, 0),
	PrevButton(true)
{
	MyShip=this;
}

void* CMyShip::operator new(size_t n) {
	return MyShipList->New(n);
}

void CMyShip::operator delete(void* p) {
	MyShipList->Delete(p);
}

void CMyShip::Move() {
	CInputState* is=&InputState;
	float 
		speed=is->Button[0]?MyShipLowSpeed:MyShipHighSpeed,
		vx=is->Left?-1:(is->Right?1:0), 
		vy=is->Up?-1:(is->Down?1:0),
		mx=MaxX-Shape->Size,
		my=MaxY-Shape->Size,
		x=X+vx*speed,
		y=Y+vy*speed;
	if (x<-mx) x=-mx; else if (x>mx) x=mx;
	if (y<-my) y=-my; else if (y>my) y=my;
	
	float dx=x-X, dy=y-Y, d=sqrtf(dx*dx+dy*dy);
	if (d>speed) {
		dx*=speed/d;
		dy*=speed/d;
	}
	X+=dx;
	Y+=dy;
	
	if (Color<0) {
		if (IsHit(BulletList) || IsHit(EnemyList)) {
			Alive=false;
			MyShip=new CMyShipCrash(this);
		}
	} else {
		if (!PrevButton && is->Button[1]) {
			Color=1-Color;
		}
		PrevButton=is->Button[1];
		SetShape(Color==0?IkBlackMyShip:IkWhiteMyShip);

		for (CTaskIter i(BulletList); i.HasNext(); ) {
			CMover* mover=static_cast<CMover*>(i.Next());
			if (mover->Color==Color) {
				float 
					dx=mover->X-X, 
					dy=mover->Y-Y,
					hit=mover->Shape->Hit+Shape->Size;
				if (dx*dx+dy*dy<hit*hit) {
					mover->Alive=false;
				}
			} else {
				if (IsHit(mover)) {
					Alive=false;
					MyShip=new CMyShipCrash(this);
					break;
				}
			}
		}
		if (IsHit(EnemyList)) {
			Alive=false;
			MyShip=new CMyShipCrash(this);
		}
	}
}

CMyShipCrash::CMyShipCrash(CMyShip* myship)
:	CMyShip(ShVoid)
{
	Shape=myship->Shape;
	X=myship->X;
	Y=myship->Y;
}

void CMyShipCrash::Move() {
	Angle+=0.08f;
	Scale-=0.04f;
	Alpha-=0.04f;
	if (Scale<0) Scale=0;
	if (Alpha<0) Alpha=0;
}

//==============================================================
// 敵
CEnemy::CEnemy(int shape_id)
:	CMover(EnemyList, shape_id, 0, -0.7f, 0)
{}

void* CEnemy::operator new(size_t n) {
	return EnemyList->New(n);
}

void CEnemy::operator delete(void* p) {
	EnemyList->Delete(p);
}

//==============================================================
// 弾

CBullet::CBullet(
	int shape_id, float x, float y, 
	float angle, float angle_rate, 
	float speed, float speed_rate)
:	CMover(BulletList, shape_id, x, y, angle),
	AngleRate(angle_rate), 
	Speed(speed), SpeedRate(speed_rate)
{}

void* CBullet::operator new(size_t n) {
	return BulletList->New(n);
}

void CBullet::operator delete(void* p) {
	BulletList->Delete(p);
}

void CBullet::Move() {
	float rad=Angle*D3DX_PI*2;
	X+=Speed*cosf(rad);
	Y+=Speed*sinf(rad);
	Angle+=AngleRate;
	Speed+=SpeedRate;
	if (abs(X)>=MaxX+Shape->Size || abs(Y)>=MaxY+Shape->Size) {
		Alive=false;
	}
}

//==============================================================
// サブルーチン

float GetMyShipAngle(float x, float y) {
	return atan2f(MyShip->Y-y, MyShip->X-x)/D3DX_PI/2;
}

float GetAngle(float x, float y, float to_x, float to_y) {
	return atan2f(to_y-y, to_x-x)/D3DX_PI/2;
}

void ShootNWay(
	int shape_id, float x, float y, float angle, float angle_range, 
	float speed, int count, float angle_rate, float speed_rate
) {
	if (count>1) {
		for (int i=0; i<count; i++) {
			new CBullet(
				shape_id, x, y, 
				angle+angle_range*((float)i/(count-1)-0.5f), 
				angle_rate, speed, speed_rate);
		}
	} else
	if (count==1) {
		new CBullet(
			shape_id, x, y, angle,
			angle_rate, speed, speed_rate);
	}
}

