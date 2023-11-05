//==============================================================
// 스케이트 보드

// 스케이트 보드
class CSkateboard : public CMover {
public:
	CSkateboard() {
		Texture=Game->Texture[TEX_SKATEBOARD];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2.4f;
		W=H=2;
		Type=1;
	}
};

// 사람
class CSkateboardMan : public CMover {
public:
	float VX;
	CSkateboard* Skateboard;
	bool PrevButton;

	CSkateboardMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		Skateboard=NULL;
		PrevButton=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.1f;
		float skate_speed=3;
		float max_dist=1.0f;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;
		if (Skateboard) VX*=skate_speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		if (!PrevButton && is->Button[0]) {
			if (Skateboard) {
				Skateboard=NULL;
			} else {
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==1 &&
						abs(X-mover->X)<max_dist
					) {
						Skateboard=(CSkateboard*)mover;
					}
				}
			}
		}
		
		if (Skateboard) {
			Skateboard->X=X;
			Y=Skateboard->Y+0.05f;
		} else {
			Y=MAX_Y-2;
		}

		PrevButton=is->Button[0];

		Angle=Skateboard?0:VX/speed*0.05f;
		return true;
	}
};

// 스테이지
class CSkateboardStage : public CStage {
public:
	CSkateboardStage() : CStage("SKATEBOARD") {}
	virtual void Init() {
		new CSkateboardMan();
		new CSkateboard();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 자동차

// 자동차
class CCar : public CMover {
public:
	float VX;
	CCar() {
		Texture=Game->Texture[TEX_CAR];
		Color=COL_MGRAY;
		W=H=2;
		X=MAX_X+W;
		Y=MAX_Y-2.4f;
		VX=Game->Rand.Real1()*-0.05f-0.05f;
		Type=1;
	}
	virtual bool Move(const CInputState* is) {
		X+=VX;
		if (X<-W) X=MAX_X+W;
		return true;
	}
};

// 사람
class CCarMan : public CMover {
public:
	float VX, VY;
	bool Jump;

	CCarMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=VY=0;
		Jump=false;
		DrawOrder=1;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float jump_speed=-0.4f;
		float jump_accel=0.02f;
		float min_x=-1.0f;
		float max_x=0.5f;
		float min_y=0.6f;
		float max_y=1.6f;
		float adjust_y=1.1f;

		if (!Jump) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0]) VY=jump_speed;
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		VY+=jump_accel;
		if (VY>-jump_speed) VY=-jump_speed;
		Y+=VY;

		Jump=true;
		if (Y>MAX_Y-2) {
			Y=MAX_Y-2;
			Jump=false;
		}
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				VY>0 &&
				mover->Type==1 &&
				mover->X-X>min_x &&
				mover->X-X<max_x &&
				mover->Y-Y>min_y &&
				mover->Y-Y<max_y
			) {
				CCar* car=(CCar*)mover;
				X+=car->VX;
				Y=car->Y-adjust_y;
				VY=0;
				Jump=false;
				break;
			}
		}
		
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CCarStage : public CStage {
public:
	CCarStage() : CStage("CAR") {}
	virtual void Init() {
		new CCarMan();
		for (int i=0; i<3; i++) new CCar();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 동물

// 동물
class CAnimal : public CMover {
public:
	CAnimal() {
		Texture=Game->Texture[TEX_ANIMAL];
		Color=COL_MGRAY;
		X=(MAX_X-1)/2;
		Y=MAX_Y-2.85f;
		W=H=3;
		Type=1;
	}
};

// 사람
class CAnimalMan : public CMover {
public:
	float VX, DirX;
	CAnimal* Animal;
	bool PrevButton;

	CAnimalMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=DirX=0;
		Animal=NULL;
		PrevButton=false;
		DrawOrder=1;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.1f;
		float animal_speed=3;
		float max_dist=1.0f;

		VX=0;
		if (is->Left) {
			DirX=-1;
			VX=-speed;
		}
		if (is->Right) {
			DirX=1;
			VX=speed;
		}
		if (Animal) VX*=animal_speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		if (!PrevButton && is->Button[0]) {
			if (Animal) {
				Animal=NULL;
			} else {
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==1 &&
						abs(X-mover->X)<max_dist
					) {
						Animal=(CAnimal*)mover;
						break;
					}
				}
			}
		}
		
		if (Animal) {
			Texture=Game->Texture[TEX_CROUCH];
			Animal->X=X+DirX*0.5f;
			Y=Animal->Y-0.5f;
			Animal->ReverseX=(DirX>0);
		} else {
			Texture=Game->Texture[TEX_MAN];
			Y=MAX_Y-2;
		}

		PrevButton=is->Button[0];

		Angle=Animal?0:VX/speed*0.05f;
		return true;
	}
};

// 스테이지
class CAnimalStage : public CStage {
public:
	CAnimalStage() : CStage("ANIMAL") {}
	virtual void Init() {
		new CAnimalMan();
		new CAnimal();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 시소

class CSeesawMan;

// 시소
class CSeesaw : public CMover {
public:
	float VX;
	CSeesawMan* Man[2];
	CSeesaw() {
		X=(MAX_X+1)/2;
		Y=MAX_Y-1.9f;
		VX=0;
		Man[0]=Man[1]=NULL;
	}
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_angle=0.05f;
		
		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Angle=0;
		if (Man[0]) Angle=-max_angle;
		if (Man[1]) Angle=max_angle;
		
		return true;
	}
	virtual void Draw() {
		float x=X, y=Y, angle=Angle;

		Texture=Game->Texture[TEX_SEESAW0];
		Y=y+0.1f;
		W=H=1;
		Angle=0;	
		CMover::Draw();

		Texture=Game->Texture[TEX_SEESAW1];
		Y=y-0.3f;
		W=4;
		Angle=angle;
		CMover::Draw();

		X=x;
		Y=y;
		Angle=angle;
	}
};

// 사람
class CSeesawMan : public CMover {
public:
	float VX, VY;
	CSeesaw* Seesaw;

	void Init() {
		X=Game->Rand.Real1()*(MAX_X-1);
		Y=-1;
		VX=VY=0;
	}

	CSeesawMan(CSeesaw* seesaw) {
		Seesaw=seesaw;
		Texture=Game->Texture[TEX_MAN];
		Init();
		DrawOrder=1;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float jump_speed=-0.4f;
		float jump_accel=0.008f;

		float max_x=2.0f;
		float min_y=0.5f;
		float max_y=1.5f;

		float ride_x=1.5f;
		float ride_y=-0.3f;

		X+=VX;
		if (X<-1) X=MAX_X;
		if (X>MAX_X) X=-1;

		VY+=jump_accel;
		if (VY>-jump_speed) VY=-jump_speed;
		
		Y+=VY;
		if (Y>MAX_Y) Init();

		if (
			VY>0 &&
			abs(Seesaw->X-X)<max_x &&
			Seesaw->Y-Y>min_y &&
			Seesaw->Y-Y<max_y
		) {
			if (!Seesaw->Man[0] && X<Seesaw->X) {
				Seesaw->Man[0]=this;
				if (Seesaw->Man[1]) {
					Seesaw->Man[1]->VX=speed*((Seesaw->X-X)/max_x-0.5f);
					Seesaw->Man[1]->VY=jump_speed;
					Seesaw->Man[1]=NULL;					
				}
			}
			if (!Seesaw->Man[1] && X>Seesaw->X) {
				Seesaw->Man[1]=this;
				if (Seesaw->Man[0]) {
					Seesaw->Man[0]->VX=speed*((Seesaw->X-X)/max_x+0.5f);
					Seesaw->Man[0]->VY=jump_speed;
					Seesaw->Man[0]=NULL;					
				}
			}
		}

		if (Seesaw->Man[0]==this) {
			X=Seesaw->X-ride_x;
			Y=Seesaw->Y+ride_y;
		}
		if (Seesaw->Man[1]==this) {
			X=Seesaw->X+ride_x;
			Y=Seesaw->Y+ride_y;
		}
		
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CSeesawStage : public CStage {
public:
	CSeesawStage() : CStage("SEESAW") {}
	virtual void Init() {
		CSeesaw* seesaw=new CSeesaw();
		for (int i=0; i<2; i++) new CSeesawMan(seesaw);
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 시계추

// 시계추
class CPendulum : public CMover {
public:
	float VAngle, RootX, RootY, Length;
	CPendulum(float root_x, float root_y, float length, float vangle) {
		RootX=root_x;
		RootY=root_y;
		Length=length;
		Type=1;
		VAngle=vangle;
		Move(NULL);
		Color=COL_MGRAY;
	}
	virtual bool Move(const CInputState* is) {
		float max_angle=0.2f;

		Angle+=VAngle;
		if (abs(Angle)>=max_angle) VAngle=-VAngle;
		
		float rad=Angle*D3DX_PI*2;
		X=RootX+sinf(rad)*Length;
		Y=RootY+cosf(rad)*Length;

		return true;
	}
	virtual void Draw() {
		float 
			w=Game->GetGraphics()->GetWidth()/MAX_X,
			h=Game->GetGraphics()->GetHeight()/MAX_Y,
			rad=Angle*D3DX_PI*2,
			c=cosf(rad)*0.05f,
			s=sinf(rad)*0.05f;
		float x[]={
			(RootX-c+0.5f)*w, (RootX+c+0.5f)*w,
			(X-c+0.5f)*w, (X+c+0.5f)*w
		};
		float y[]={
			(RootY+s+0.5f)*h, (RootY-s+0.5f)*h,
			(Y+s+0.5f)*h, (Y-s+0.5f)*h
		};
		Game->Texture[TEX_FILL]->Draw(
			x[0], y[0], Color, 0, 0, 
			x[1], y[1], Color, 0, 0, 
			x[2], y[2], Color, 0, 0, 
			x[3], y[3], Color, 0, 0
		);
		Texture=Game->Texture[TEX_BALL];
		CMover::Draw();
	}
};

// 사람
class CPendulumMan : public CMover {
public:
	float VX, VY;
	bool Jump, PrevButton;
	CPendulum* Pendulum;
	CPendulum* PrevPendulum;

	CPendulumMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=VY=0;
		Jump=false;
		PrevButton=false;
		Pendulum=PrevPendulum=NULL;
		DrawOrder=1;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float jump_speed=-0.4f;
		float jump_accel=0.02f;
		float max_dist=1.0f;

		if (!Jump) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (!PrevButton && is->Button[0]) {
				VY=jump_speed;
				Pendulum=NULL;
				Jump=true;
			}
		}
		PrevButton=is->Button[0];

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		VY+=jump_accel;
		if (VY>-jump_speed) VY=-jump_speed;
		Y+=VY;

		if (Y>=MAX_Y-2) {
			Y=MAX_Y-2;
			Jump=false;
		}

		if (
			PrevPendulum &&
			(abs(PrevPendulum->X-X)>=max_dist ||
			abs(PrevPendulum->Y-Y)>=max_dist)
		) {
			PrevPendulum=NULL;
		}
				
		if (Pendulum) {
			X=Pendulum->X;
			Y=Pendulum->Y;
		} else {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					abs(mover->X-X)<max_dist &&
					abs(mover->Y-Y)<max_dist &&
					mover!=PrevPendulum
				) {
					Pendulum=PrevPendulum=(CPendulum*)mover;
					Jump=false;
					break;
				}
			}
		}
		
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CPendulumStage : public CStage {
public:
	CPendulumStage() : CStage("PENDULUM") {}
	virtual void Init() {
		new CPendulumMan();
		new CPendulum(5, 1, 6, 0.003f);
		new CPendulum(11, 1, 6, 0.002f);
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 웅크리기

// 사람
class CCrouchMan : public CMover {
public:
	float VX;

	CCrouchMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float crouch_speed=0.3f;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;
		
		if (is->Down) {
			Texture=Game->Texture[TEX_CROUCH];
			VX*=crouch_speed;
			W=H=0.8f;
			Y=MAX_Y-1.8f;
		} else {
			Texture=Game->Texture[TEX_MAN];
			W=H=1;
			Y=MAX_Y-2;
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CCrouchStage : public CStage {
public:
	CCrouchStage() : CStage("CROUCH") {}
	virtual void Init() {
		new CCrouchMan();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 구르기

// 사람
class CCurlUpMan : public CMover {
public:
	float VX;
	bool CurlUp;

	CCurlUpMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		CurlUp=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float vangle=0.05f;

		if (!CurlUp) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Down) CurlUp=true;
			Texture=Game->Texture[TEX_MAN];
			W=H=1;
			Y=MAX_Y-2;
			Angle=VX/speed*0.1f;
		} else {
			if (!is->Down) CurlUp=false;
			Texture=Game->Texture[TEX_CROUCH];
			W=H=0.8f;
			Y=MAX_Y-1.8f;
			Angle+=vangle*VX/speed;
		}
		
		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		return true;
	}
};

// 스테이지
class CCurlUpStage : public CStage {
public:
	CCurlUpStage() : CStage("CURLING UP") {}
	virtual void Init() {
		new CCurlUpMan();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 거대화

// 사람
class CGiantMan : public CMover {
public:
	float VX, Size, VSize;
	bool PrevButton;

	CGiantMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		Size=1;
		VSize=0;
		PrevButton=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.1f;
		float vsize=0.05f;

		VX=0;
		if (is->Left) VX=-speed*W;
		if (is->Right) VX=speed*W;
		
		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		if (!PrevButton && is->Button[0]) {
			if (Size==1) VSize=vsize; else VSize=-vsize;
		}
		PrevButton=is->Button[0];
		
		Size+=VSize;
		W=H=Size;
		Y-=VSize/2;
		
		if (Size<=1) {
			Size=W=H=1;
			Y=MAX_Y-2;
		}
		if (Size>=4) {
			Size=W=H=4;
			Y=MAX_Y-3.5f;
		}
		
		Angle=VX/speed/W*0.1f;
		return true;
	}
};

// 스테이지
class CGiantStage : public CStage {
public:
	CGiantStage() : CStage("GIANT") {}
	virtual void Init() {
		new CGiantMan();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 복수 캐릭터 조작

// 사람
class CMultipleCharacterMan : public CMover {
public:
	float VX, VY;
	bool UseButton;

	CMultipleCharacterMan(float x, float y, bool use_button) {
		Texture=Game->Texture[TEX_MAN];
		X=x;
		Y=y;
		VX=VY=0;
		UseButton=use_button;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;

		if (is->Button[0]==UseButton) {
			VX=VY=0;
			if (is->Left) VX=-speed; else
			if (is->Right) VX=speed; else
			if (is->Up) VY=-speed; else
			if (is->Down) VY=speed;

			X+=VX;
			if (X<0) X=0;
			if (X>MAX_X-1) X=MAX_X-1;

			Y+=VY;
			if (Y<0) Y=0;
			if (Y>MAX_Y-1) Y=MAX_Y-1;
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CMultipleCharacterStage : public CStage {
public:
	CMultipleCharacterStage() : CStage("MULTIPLE CHARACTER") {}
	virtual void Init() {
		new CMultipleCharacterMan(5, 6, false);
		new CMultipleCharacterMan(11, 6, true);
	}
};


