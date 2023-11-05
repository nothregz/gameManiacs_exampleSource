//==============================================================
// 레버 대시

// 사람
class CLeverDashMan : public CMover {
public:
	float VX;

	CLeverDashMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float max_speed=0.5f;
		float accel=0.01f;

		if (is->Right) {
			VX+=accel;
		} else
		if (is->Left) {
			VX-=accel;
		} else {
			VX-=accel*0.5f;
		}

		if (VX<0) VX=0;
		if (VX>max_speed) VX=max_speed;

		Angle=VX/max_speed*0.1f;

		return true;
	}
};

// 플랫폼
class CStaticFloor : public CMover {
public:
	CStaticFloor(float x) {
		Texture=Game->Texture[TEX_FLOOR];
		X=x;
		Y=MAX_Y-1;
	}
	CStaticFloor(float x, float y) {
		Texture=Game->Texture[TEX_FLOOR];
		X=x;
		Y=y;
	}
	CStaticFloor(float x, float y, int type) {
		Texture=Game->Texture[TEX_FLOOR];
		X=x;
		Y=y;
		Type=type;
	}
	virtual bool Move(const CInputState* is) {
		if (X<=-1) X+=MAX_X+1;
		return true;
	}
};

// 나무
class CStaticTree : public CMover {
public:
	CStaticTree() {
		Texture=Game->Texture[TEX_TREE];
		Color=COL_MGRAY;
		W=H=3;
		X=MAX_X+W;
		Y=MAX_Y-H;
	}
	virtual bool Move(const CInputState* is) {
		return X>-W;
	}
};

// 스테이지
class CLeverDashStage : public CStage {
	CLeverDashMan* Man;
	float Dist;
public:
	CLeverDashStage() : CStage("LEVER DASH") {}
	virtual void Init() {
		Man=new CLeverDashMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
		Dist=0;
	}
	virtual bool Move(const CInputState* is) {
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover!=Man) mover->X-=Man->VX;
		}

		Dist-=Man->VX;
		if (Dist<0) {
			new CStaticTree();
			Dist=Game->Rand.Real1()*MAX_X;
		}
		return true;
	}
};

//==============================================================
// 버튼 대시

class CButtonDashMan : public CMover {
public:
	float VX, LeverVX, ButtonVX;

	CButtonDashMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		LeverVX=0;
		ButtonVX=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float button_max_speed=0.3f;
		float button_accel=0.006f;
		float lever_max_speed=0.2f;
		float lever_accel=0.004f;

		if (is->Right) {
			LeverVX+=lever_accel;
		} else
		if (is->Left) {
			LeverVX-=lever_accel;
		} else {
			LeverVX-=lever_accel*0.5f;
		}

		if (LeverVX<0) LeverVX=0;
		if (LeverVX>lever_max_speed) LeverVX=lever_max_speed;

		if (is->Button[0]) {
			ButtonVX+=button_accel;
		} else {
			ButtonVX-=button_accel;
		}
		
		if (ButtonVX<0) ButtonVX=0;
		if (ButtonVX>button_max_speed) ButtonVX=button_max_speed;

		VX=LeverVX+ButtonVX;

		Angle=VX/(lever_max_speed+button_max_speed)*0.1f;
		return true;
	}
};

// 스테이지
class CButtonDashStage : public CStage {
	CButtonDashMan* Man;
	float Dist;
public:
	CButtonDashStage() : CStage("BUTTON DASH") {}
	virtual void Init() {
		Man=new CButtonDashMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
		Dist=0;
	}
	virtual bool Move(const CInputState* is) {
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover!=Man) mover->X-=Man->VX;
		}

		Dist-=Man->VX;
		if (Dist<0) {
			new CStaticTree();
			Dist=Game->Rand.Real1()*MAX_X;
		}
		return true;
	}
};

//==============================================================
// 레버 2단 대시

class CDoubleLeverDashMan : public CMover {
public:
	float VX;
	int Time;
	bool Dash, PrevRight;

	CDoubleLeverDashMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		Time=60;
		Dash=false;
		PrevRight=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float dash_speed=0.6f;
		float normal_speed=0.2f;
		int dash_time=20;

		if (Dash) {
			if (!is->Right) {
				Dash=false;
				VX=0;
			}
		} else {
			if (is->Right) {
				if (!PrevRight) {
					if (Time<dash_time) {
						Dash=true;
						VX=dash_speed;
					} else {
						VX=normal_speed;
					}
					Time=0;
				}
			} else {
				VX=0;
			}
		}
		Time++;
		PrevRight=is->Right;

		Angle=VX/dash_speed*0.1f;
		return true;
	}
};

// 스테이지
class CDoubleLeverDashStage : public CStage {
	CDoubleLeverDashMan* Man;
	float Dist;
public:
	CDoubleLeverDashStage() : CStage("DOUBLE LEVER DASH") {}
	virtual void Init() {
		Man=new CDoubleLeverDashMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
		Dist=0;
	}
	virtual bool Move(const CInputState* is) {
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover!=Man) mover->X-=Man->VX;
		}

		Dist-=Man->VX;
		if (Dist<0) {
			new CStaticTree();
			Dist=Game->Rand.Real1()*MAX_X;
		}
		return true;
	}
};

//==============================================================
// 연타 대시

class CRapidButtonDashMan : public CMover {
public:
	float VX;
	bool PrevButton;

	CRapidButtonDashMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		PrevButton=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float max_speed=0.6f;
		float accel=0.03f;

		if (!PrevButton && is->Button[0]) {
			VX+=accel;
		} else {
			VX-=accel*0.1f;
		}

		if (VX<0) VX=0;
		if (VX>max_speed) VX=max_speed;		
		
		PrevButton=is->Button[0];

		Angle=VX/max_speed*0.1f;
		return true;
	}
};

// 스테이지
class CRapidButtonDashStage : public CStage {
	CRapidButtonDashMan* Man;
	float Dist;
public:
	CRapidButtonDashStage() : CStage("RAPID BUTTON DASH") {}
	virtual void Init() {
		Man=new CRapidButtonDashMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
		Dist=0;
	}
	virtual bool Move(const CInputState* is) {
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover!=Man) mover->X-=Man->VX;
		}

		Dist-=Man->VX;
		if (Dist<0) {
			new CStaticTree();
			Dist=Game->Rand.Real1()*MAX_X;
		}
		return true;
	}
};

//==============================================================
// 가속 아이템

class CSpeedUpItemMan : public CMover {
public:
	float VX;
	int ItemTime;

	CSpeedUpItemMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		ItemTime=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float item_speed=0.6f;
		float normal_speed=0.2f;
		float item_time=60;

		if (ItemTime>0) {
			VX=item_speed;
			ItemTime--;
		} else {
			VX=normal_speed;
		}
		if (!is->Right) VX=0;
		
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 && 
				abs(mover->X-X)<1.0f && 
				abs(mover->Y-Y)<1.0f
			) {
				ItemTime=item_time;
				i.Remove();
			}
		}

		Angle=VX/item_speed*0.1f;
		return true;
	}
};

// 아이템
class CStaticItem : public CMover {
public:
	CStaticItem() {
		Texture=Game->Texture[TEX_ITEM];
		X=MAX_X+1;
		Y=MAX_Y-2;
		Type=1;
		DrawOrder=1;
		Color=COL_BLACK;
	}
	virtual bool Move(const CInputState* is) {
		return X>-W;
	}
};

// 스테이지
class CSpeedUpItemStage : public CStage {
	CSpeedUpItemMan* Man;
	float Dist, ItemDist;
public:
	CSpeedUpItemStage() : CStage("SPEED UP ITEM") {}
	virtual void Init() {
		Man=new CSpeedUpItemMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
		Dist=0;
	}
	virtual bool Move(const CInputState* is) {
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover!=Man) mover->X-=Man->VX;
		}

		Dist-=Man->VX;
		if (Dist<0) {
			new CStaticTree();
			Dist=Game->Rand.Real1()*MAX_X;
		}
		ItemDist-=Man->VX;
		if (ItemDist<0) {
			new CStaticItem();
			ItemDist=Game->Rand.Real1()*MAX_X*3;
		}
		return true;
	}
};

//==============================================================
// 얼음에서 미끄러지기

class CSlipOnIceMan : public CMover {
public:
	float VX;
	int SlipTime;

	CSlipOnIceMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		SlipTime=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.1f;
		int slip_time=30;

		if (SlipTime>0) {
			SlipTime--;
			if (SlipTime==0) VX=0;
		} else {
			if (VX>0) {
				if (is->Left || !is->Right) SlipTime=slip_time;
			} else
			if (VX<0) {
				if (is->Right || !is->Left) SlipTime=slip_time;
			} else
			{
				if (is->Left) VX=-speed;
				if (is->Right) VX=speed;
			}
		}
		
		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		if (SlipTime>0) Angle=-Angle; else Angle=VX*0.5f;
		
		return true;
	}
};

// 얼음
class CStaticIce : public CMover {
public:
	CStaticIce(float x) {
		Texture=Game->Texture[TEX_ICE_FLOOR];
		Color=COL_WHITE;
		X=x;
		Y=MAX_Y-1;
	}
};


// 스테이지
class CSlipOnIceStage : public CStage {
	CSlipOnIceMan* Man;
public:
	CSlipOnIceStage() : CStage("SLIP ON ICE") {}
	virtual void Init() {
		Man=new CSlipOnIceMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticIce(i);
	}
};

//==============================================================
// 헤엄치기

class CSwimmingMan : public CMover {
public:
	float VX, VY;
	bool PrevButton;
	int Time;

	CSwimmingMan() {
		Texture=Game->Texture[TEX_SWIMMER];
		X=3;
		Y=(MAX_Y-1)/2;
		VX=0;
		VY=0;
		PrevButton=false;
		Time=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float x_speed=0.05f;
		float up_speed=-0.05f;
		float down_speed=0.03f;
		float up_time=20;

		VX=0;
		if (is->Left) VX=-x_speed;
		if (is->Right) VX=x_speed;

		if (!PrevButton && is->Button[0]) Time=up_time;		
		PrevButton=is->Button[0];

		if (Time>0) {
			VY=up_speed;
			Time--;
		} else {
			VY=down_speed;
		}
		
		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		Y+=VY;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;

		if (ReverseX && VX>0) ReverseX=false;
		if (!ReverseX && VX<0) ReverseX=true;
		return true;
	}
};

// 물고기
class CSwimmingFish : public CMover {
public:
	float VX;
	CSwimmingFish() {
		float speed=0.1f;
		Texture=Game->Texture[TEX_FISH];
		Color=COL_MGRAY;
		X=-1;
		Y=Game->Rand.Real1()*MAX_Y;
		VX=(Game->Rand.Real1()+0.1f)*speed;
		ReverseX=true;
		if (Game->Rand.Int32()%2==0) {
			X=MAX_X;
			VX=-VX;
			ReverseX=false;
		}
	}
	virtual bool Move(const CInputState* is) {
		X+=VX;
		if (X<=-1) X+=MAX_X+1;
		if (X>=MAX_X) X-=MAX_X+1;
		return true;
	}
};


// 스테이지
class CSwimmingStage : public CStage {
	CSwimmingMan* Man;
public:
	CSwimmingStage() : CStage("SWIMMING") {}
	virtual void Init() {
		Man=new CSwimmingMan();
	}
	virtual bool Move(const CInputState* is) {
		if (Game->Rand.Real1()<0.05f) new CSwimmingFish();
		return true;
	}
};

//==============================================================
// 라인 이동

class CLineMoveMan : public CMover {
public:
	float VX, VY;

	CLineMoveMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-4*0.8f;
		VX=0;
		VY=0;
		DrawOrder=1;
	}
	
	virtual bool Move(const CInputState* is) {
		float x_speed=0.07f;
		float y_speed=0.05f;
		float yx_ratio=0.4f;
		float min_y=MAX_Y-6*0.8f-0.2f;
		float max_y=MAX_Y-1-0.2f;
		
		VX=0;
		VY=0;
		
		if (is->Left) VX=-x_speed;
		if (is->Right) VX=x_speed;
		
		if (is->Up && Y>min_y) {
			VX+=y_speed*yx_ratio;
			VY=-y_speed;
		}
		if (is->Down && Y<max_y) {
			VX-=y_speed*yx_ratio;
			VY=y_speed;		
		}
		
		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Y+=VY;

		return true;
	}
};

// 플랫폼
class CLineMoveFloor : public CMover {
public:
	CLineMoveFloor(float x, float y) {
		Texture=Game->Texture[TEX_FLOOR];
		X=x;
		Y=y;
		ShearX=0.2f;
		Color=COL_MGRAY;
	}
	virtual bool Move(const CInputState* is) {
		if (X<=-1) X+=MAX_X+1;
		return true;
	}
};

// 스테이지
class CLineMoveStage : public CStage {
	CLineMoveMan* Man;
public:
	CLineMoveStage() : CStage("LINE MOVE") {}
	virtual void Init() {
		Man=new CLineMoveMan();
		for (int j=0; j<5; j++) {
			for (int i=-1; i<MAX_X+1; i++) {
				new CLineMoveFloor(i-j*0.2f, MAX_Y-1-j*0.8f);
			}
		}
	}
};

//==============================================================
// 화면 끝 워프

class CScreenEdgeWarpMan : public CMover {
public:
	CScreenEdgeWarpMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=(MAX_Y-1)/2;
		DrawOrder=1;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.08f;
		
		if (is->Left) X-=speed;
		if (is->Right) X+=speed;
		if (is->Up) Y-=speed;
		if (is->Down) Y+=speed;
		
		if (X<=-1) X+=MAX_X;
		if (X>MAX_X-1) X-=MAX_X; 
		if (Y<=-1) Y+=MAX_Y;
		if (Y>MAX_Y-1) Y-=MAX_Y; 
		
		return true;
	}
	
	virtual void Draw() {
		for (int i=0; i<2; i++) {
			for (int j=0; j<2; j++) {
				float x=X, y=Y;
				X+=j*MAX_X;
				Y+=i*MAX_Y;
				CMover::Draw();
				X=x;
				Y=y;
			}
		}
	}
};

// 나무
class CScreenEdgeWarpTree : public CMover {
public:
	CScreenEdgeWarpTree() {
		Texture=Game->Texture[TEX_TREE];
		Color=COL_MGRAY;
		W=H=3;
		X=Game->Rand.Real1()*MAX_X;
		Y=Game->Rand.Real1()*MAX_Y;
	}
};

// 스테이지
class CScreenEdgeWarpStage : public CStage {
	CScreenEdgeWarpMan* Man;
public:
	CScreenEdgeWarpStage() : CStage("SCREEN EDGE WARP") {}
	virtual void Init() {
		Man=new CScreenEdgeWarpMan();
		for (int i=0; i<20; i++) new CScreenEdgeWarpTree();
	}
};

//==============================================================
// 이동하면 생명이 줄어든다

class CDecreaseLifeMan : public CMover {
public:
	float Life;
	float VX, VY;

	CDecreaseLifeMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=(MAX_Y-1)/2;
		Life=1;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.05f;
		float dec_life=0.005f;
		float add_life=0.01f;
		
		VX=VY=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;
		if (is->Up) VY=-speed;
		if (is->Down) VY=speed;
		
		if (Life>0) {
			X+=VX;
			if (X<0) X=0;
			if (X>MAX_X-1) X=MAX_X-1;

			Y+=VY;
			if (Y<0) Y=0;
			if (Y>MAX_Y-3) Y=MAX_Y-3;
		}

		if (VX!=0 || VY!=0) Life-=dec_life; else Life+=add_life;
		if (Life<0) Life=0;
		if (Life>1) Life=1;
		
		return true;
	}
};

// 스테이지
class CDecreaseLifeStage : public CStage {
	CDecreaseLifeMan* Man;
public:
	CDecreaseLifeStage() : CStage("DECREASING LIFE") {}
	virtual void Init() {
		Man=new CDecreaseLifeMan();
	}
	virtual void Draw() {
		float sw=Game->GetGraphics()->GetWidth();
		float sh=Game->GetGraphics()->GetHeight();
		Game->Font->DrawText("LIFE", 0, sh*(MAX_Y-2)/MAX_Y, COL_MGRAY);
		CTexture::DrawRect(Game->GetGraphics()->GetDevice(), 0, sh*(MAX_Y-1)/MAX_Y, sw*Man->Life, sh/20, COL_MGRAY);
	}
};

//==============================================================
// 레버 입력과 반대 방향으로 움직이기

class CReverseDirectionItem : public CMover {
public:
	CReverseDirectionItem() {
		Texture=Game->Texture[TEX_ITEM];
		X=Game->Rand.Real1()*(MAX_X-1);
		Y=Game->Rand.Real1()*(MAX_Y-1);
		Type=1;
		Color=COL_BLACK;
	}
};

class CReverseDirectionMan : public CMover {
public:
	bool Reverse;

	CReverseDirectionMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		Reverse=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.1f;
		
		if (Reverse) speed=-speed;
		if (is->Left) X-=speed;
		if (is->Right) X+=speed;
		if (is->Up) Y-=speed;
		if (is->Down) Y+=speed;
		
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;

		bool get_item=false;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 && 
				abs(mover->X-X)<1.0f && 
				abs(mover->Y-Y)<1.0f
			) {
				Reverse=!Reverse;
				i.Remove();
				get_item=true;
			}
		}
		if (get_item) new CReverseDirectionItem();

		Angle=Reverse?0.5f:0;
		return true;
	}
};

// 스테이지
class CReverseDirectionStage : public CStage {
	CReverseDirectionMan* Man;
public:
	CReverseDirectionStage() : CStage("REVERSED DIRECTION") {}
	virtual void Init() {
		Man=new CReverseDirectionMan();
		for (int i=0; i<10; i++) new CReverseDirectionItem();
	}
};

//==============================================================
// 루프

// 사람
class CLoopMan : public CMover {
public:
	float VX;
	CMover* Loop;
	float LoopAngle;

	CLoopMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		Loop=NULL;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float loop_speed=0.01f;
		float loop_y=0.5f;
		float loop_radius=2.4f;

		if (Loop) {
			X=Loop->X+loop_radius*cosf((-LoopAngle+0.25f)*D3DX_PI*2);
			Y=Loop->Y+loop_y+loop_radius*sinf((-LoopAngle+0.25f)*D3DX_PI*2);
			
			LoopAngle+=loop_speed;
			if (LoopAngle>1) {
				X=Loop->X+speed;
				Y=MAX_Y-2;
				Loop=NULL;
			}

			Angle=-LoopAngle;
		} else
		{
			VX=0;
			if (is->Right) VX=speed;

			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 && 
					X<mover->X && 
					X>=mover->X-speed
				) {
					VX=0;
					Loop=mover;
					LoopAngle=0;
					break;
				}
			}
			Angle=VX/speed*0.05f;
		}

		return true;
	}
};

// 루프
class CLoopLoop : public CMover {
public:
	CLoopLoop() {
		Texture=Game->Texture[TEX_LOOP];
		W=H=7;
		X=MAX_X+W;
		Y=MAX_Y-5;
		Type=1;
	}
	virtual bool Move(const CInputState* is) {
		return X>-W;
	}
};

// 스테이지
class CLoopStage : public CStage {
	CLoopMan* Man;
	float Dist;
public:
	CLoopStage() : CStage("LOOP") {}
	virtual void Init() {
		Man=new CLoopMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
		Dist=0;
	}
	virtual bool Move(const CInputState* is) {
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover!=Man) mover->X-=Man->VX;
		}

		Dist-=Man->VX;
		if (Dist<0) {
			new CLoopLoop();
			Dist=(Game->Rand.Real1()+0.8f)*MAX_X;
		}
		return true;
	}
};

