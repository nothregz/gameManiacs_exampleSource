//==============================================================
// 로프

// 사람
class CRopeMan : public CMover {
public:
	float VX, VY;

	CRopeMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=3;
		VX=0;
		VY=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_x=0.8f;
		float min_y=0.0f;
		float max_y=0.2f;
		
		if (VY==0) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;

			X+=VX;
			if (X<0) X=0;
			if (X>MAX_X-1) X=MAX_X-1;
		}

		VY=speed;
		if (!is->Down) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					abs(mover->X-X)<max_x &&
					mover->Y-Y>=min_y && mover->Y-Y<max_y
				) {
					VY=0;
					Y=mover->Y;
					break;
				}
			}
		}
		
		Y+=VY;
		if (Y>MAX_Y) Y=-1;

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 로프
class CRope : public CMover {
public:
	CRope(float x, float y) {
		Texture=Game->Texture[TEX_ROPE];
		X=x;
		Y=y;
		Type=1;
	}
};

// 스테이지
class CRopeStage : public CStage {
	CRopeMan* Man;
public:
	CRopeStage() : CStage("ROPE") {}
	virtual void Init() {
		Man=new CRopeMan();
		for (int j=0; j<4; j++) {
			for (int i=j*2; i<MAX_X+1-j*2; i++) new CRope(i, MAX_Y-2-j*3);
		}
	}
};

//==============================================================
// 사다리

// 사람
class CLadderMan : public CMover {
public:
	float VX, VY;
	bool Fall;

	CLadderMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=3;
		VX=0;
		VY=0;
		Fall=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float ground_y=MAX_Y-2;
		float max_x=0.6f;
		float min_y=-1.0f;
		float max_y=1.0f;
		
		bool ladder=false;
		bool floor=false;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(mover->X-X)<max_x &&
				mover->Y-Y>=min_y && mover->Y-Y<max_y
			) {
				if (mover->Y-Y<max_y-speed) ladder=true; else floor=true;
				break;
			}
		}
		
		if (Y>=ground_y) {
			Y=ground_y;
			floor=true;
		}

		if (ladder || floor) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			X+=VX;
			if (X<0) X=0;
			if (X>MAX_X-1) X=MAX_X-1;

			VY=0;
			if (ladder && is->Up) VY=-speed;
			if (is->Down) VY=speed;		
			Y+=VY;
			if (Y<0) Y=0;
			if (Y>ground_y) Y=ground_y;
		}
		
		if (!ladder && !floor) {
			Y+=speed;
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 사다리
class CLadder : public CMover {
public:
	CLadder(float x, float y) {
		Texture=Game->Texture[TEX_LADDER];
		X=x;
		Y=y;
		Type=1;
	}
};

// 스테이지
class CLadderStage : public CStage {
	CLadderMan* Man;
public:
	CLadderStage() : CStage("LADDER") {}
	virtual void Init() {
		Man=new CLadderMan();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
		for (int j=0; j<5; j++) {
			for (int i=j*2+2; i<MAX_Y-1; i++) new CLadder(j*3+1, i);
		}
	}
};

//==============================================================
// 트램펄린

// 사람
class CTrampolineMan : public CMover {
public:
	float VX, VY;
	bool Trampoline;

	CTrampolineMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		Trampoline=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		int trampoline_x=5;
		int floor_y=3;
		
		if (!Trampoline) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			X+=VX;
			for (int i=0; i<4; i++) {
				float tx=i*5;
				if (abs(X-tx)<0.1f) {
					X=tx;
					VX=0;
					VY=speed;
					Trampoline=true;
					break;
				}
			}
		} else {
			Y+=VY;
			if (Y<=0) VY=speed;
			if (Y>=MAX_Y-2) VY=-speed;
			for (int i=0; i<4; i++) {
				float fy=MAX_Y-2-i*3;
				if (
					VY<0 &&
					abs(Y-fy)<0.1f &&
					(X>0 && is->Left || X<MAX_X-1 && is->Right)
				) {
					Y=fy;
					VY=0;
					Trampoline=false;
					break;
				}
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 트램펄린
class CTrampoline : public CMover {
	CTrampolineMan* Man;
	int State, Time;
public:
	CTrampoline(float x, float y, CTrampolineMan* man) {
		Texture=Game->Texture[TEX_TRAMPOLINE0];
		X=x;
		Y=y;
		Man=man;
		Type=1;
		State=0;
	}
	virtual bool Move(const CInputState* is) {
		switch (State) {
			case 0:
				Texture=Game->Texture[TEX_TRAMPOLINE0];
				if (X==Man->X && Y-Man->Y<2.0f) {
					Time=10;
					State=1;
				}
				break;
			case 1:
				Texture=Game->Texture[TEX_TRAMPOLINE1];
				Time--;
				if (Time==0) {
					Time=20;
					State=2;
				}
				break;
			case 2:
				Texture=Game->Texture[TEX_TRAMPOLINE2];
				Time--;
				if (Time==0) {
					State=0;
				}
				break;
		}
		return true;
	}
};

// 스테이지
class CTrampolineStage : public CStage {
	CTrampolineMan* Man;
public:
	CTrampolineStage() : CStage("TRAMPOLINE") {}
	virtual void Init() {
		Man=new CTrampolineMan();
		for (int j=0; j<4; j++) {
			for (int i=0; i<MAX_X; i++) {
				if (i%5!=0) new CStaticFloor(i, MAX_Y-1-j*3);
			}
		}
		for (int i=0; i<4; i++) new CTrampoline(i*5, MAX_Y-1, Man);
	}
};

//==============================================================
// 밟으면 떨어지는 플랫폼

// 사람
class CDroppingFloorMan : public CMover {
public:
	float VX, VY;

	CDroppingFloorMan() {
		Texture=Game->Texture[TEX_MAN];
		X=0;
		Y=0;
		VX=0;
		VY=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_x=0.8f;
		float min_y=0.0f;
		float max_y=1.0f;
		
		if (VY==0) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;

			X+=VX;
			if (X<0) X=0;
			if (X>MAX_X-1) X=MAX_X-1;
		}

		VY=speed;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(mover->X-X)<max_x &&
				mover->Y-Y>=min_y && mover->Y-Y<max_y
			) {
				VY=0;
				break;
			}
		}
		
		Y+=VY;
		if (Y>MAX_Y) Y=-1;

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 밟으면 떨어지는 플랫폼
class CDroppingFloor : public CMover {
	CDroppingFloorMan* Man;
	bool Drop;
	float VY;
public:
	CDroppingFloor(float x, float y, CDroppingFloorMan* man) {
		Texture=Game->Texture[TEX_DROP_FLOOR];
		X=x;
		Y=y;
		VY=0;
		Man=man;
		Type=1;
		Drop=false;
	}
	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		float max_x=0.4f;
		float min_y=0.0f;
		float max_y=1.1f;
		if (!Drop) {
			if (
				abs(X-Man->X)<max_x &&
				Y-Man->Y>min_y && Y-Man->Y<max_y
			) {
				Drop=true;
			}
		} else {
			VY+=accel;
			Y+=VY;
			if (Y>MAX_Y) return false;
		}
		return true;
	}
};

// 스테이지
class CDroppingFloorStage : public CStage {
	CDroppingFloorMan* Man;
public:
	CDroppingFloorStage() : CStage("DROPPING FLOOR") {}
	virtual void Init() {
		Man=new CDroppingFloorMan();
		for (int j=0; j<3; j++) {
			for (int i=0; i<MAX_X; i++) {
				if (i>j*3+1 && i<MAX_X-j*3-1 || (i+j)%4==0) {
					new CDroppingFloor(i, MAX_Y-1-j*4, Man);
				} else {
					CMover* mover=new CStaticFloor(i, MAX_Y-1-j*4);
					mover->Type=1;
				}
			}
		}
	}
};

//==============================================================
// 회전문

// 사람
class CRevolvingDoorMan : public CMover {
public:
	float VX, VY;

	CRevolvingDoorMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=(MAX_Y-1)/2;
		VX=0;
		VY=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		VY=0;
		if (is->Up) VY=-speed;
		if (is->Down) VY=speed;
		
		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Y+=VY;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;

		return true;
	}
};

// 회전문
class CRevolvingDoor : public CMover {
	CRevolvingDoorMan* Man;
	float VAngle, DAngle;
public:
	CRevolvingDoor(float x, float y, CRevolvingDoorMan* man) {
		Texture=Game->Texture[TEX_REVOLVING_DOOR];
		X=x;
		Y=y;
		W=H=3;
		Man=man;
		DAngle=0;
		VAngle=0;
	}
	virtual bool Move(const CInputState* is) {
		float vangle=0.02f;
		float dist_short=1.0f;
		float dist_long=2.0f;
		
		if (VAngle==0) {
			if (
				Angle==0 &&
				abs(Man->X-X)<dist_short &&
				abs(Man->Y-Y)<dist_long
			) {
				if (Man->VX<0) VAngle=-vangle;
				if (Man->VX>0) VAngle=vangle;
				if (Man->Y>Y) VAngle=-VAngle;
			}
			if (
				Angle==0.25f &&
				abs(Man->X-X)<dist_long &&
				abs(Man->Y-Y)<dist_short
			) {
				if (Man->VY<0) VAngle=vangle;
				if (Man->VY>0) VAngle=-vangle;				
				if (Man->X>X) VAngle=-VAngle;
			}
		} else {
			Angle+=VAngle;
			DAngle+=abs(VAngle);
			if (DAngle>=0.25f) {
				if (abs(Angle)<0.1f || abs(Angle)>0.4f) Angle=0; else Angle=0.25f;
				DAngle=0;
				VAngle=0;
			}
		}

		return true;
	}
};

// 스테이지
class CRevolvingDoorStage : public CStage {
	CRevolvingDoorMan* Man;
public:
	CRevolvingDoorStage() : CStage("REVOLVING DOOR") {}
	virtual void Init() {
		Man=new CRevolvingDoorMan();
		for (int j=0; j<2; j++) {
			for (int i=0; i<3; i++) {
				new CRevolvingDoor(i*5+2, j*5+3, Man);
			}
		}
	}
};

//==============================================================
// 문 밀치기

// 사람
class CDoorSmashMan : public CMover {
public:

	CDoorSmashMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
	}
	
	virtual bool Move(const CInputState* is) {
		return true;
	}
};

// 문
class CDoorSmashDoor : public CMover {
public:
	int State, Time;
	bool PrevButton;
	CDoorSmashDoor() {
		Texture=Game->Texture[TEX_DOOR1];
		W=H=2;
		X=5;
		Y=MAX_Y-2.5f;
		State=0;
		PrevButton=false;
	}
	virtual bool Move(const CInputState* is) {
		int smash_time=10;
		switch (State) {
			case 0:
				Texture=Game->Texture[TEX_DOOR1];
				if (!PrevButton && is->Button[0]) State=1;
				break;
			case 1:
				Texture=Game->Texture[TEX_DOOR0];
				if (!PrevButton && is->Button[0]) {
					Time=smash_time;
					State=2;
				}
				break;
			case 2:
				Texture=Game->Texture[TEX_DOOR1];
				if (!PrevButton && is->Button[0]) State=1;
				Time--;
				if (Time==0) State=0;
				break;
		}
		PrevButton=is->Button[0];
		return true;
	}
};

// 적
class CDoorSmashEnemy : public CMover {
	float VX;
	int State, Time;
	CDoorSmashDoor* Door;
public:	
	CDoorSmashEnemy(CDoorSmashDoor* door) {
		Texture=Game->Texture[TEX_ENEMY0];
		X=MAX_X;
		Y=MAX_Y-2;
		VX=-Game->Rand.Real1()*0.1f-0.05f;
		State=0;
		Door=door;
		Color=COL_MGRAY;
		DrawOrder=1;
	}
	virtual bool Move(const CInputState* is) {
		float max_x=1.0f;
		float smashed_speed=0.5f;
		float smashed_time=10;
		float sleep_time=60;
		
		switch (State) {
			case 0:
				if (
					Door->State==1 ||
					Door->X-X>0 || X-Door->X>max_x
				) {
					X+=VX;
					if (X<-1) X=MAX_X;
				}
				if (
					Door->State==2 &&
					abs(Door->X-X)<max_x
				) {
					Time=smashed_time;
					State=1;
				}
				break;
			case 1:
				X+=smashed_speed;
				Time--;
				if (Time==0) {
					Time=sleep_time;
					State=2;
				}
				break;
			case 2:
				Time--;
				if (Time==0) State=0;
				break;
		}
		
		Angle=(State==0)?0:0.5f;
		return true;
	}
};

// 스테이지
class CDoorSmashStage : public CStage {
	CDoorSmashMan* Man;
	CDoorSmashDoor* Door;
public:
	CDoorSmashStage() : CStage("DOOR SMASH") {}
	virtual void Init() {
		Man=new CDoorSmashMan();
		Door=new CDoorSmashDoor();
		for (int i=0; i<3; i++) new CDoorSmashEnemy(Door);
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
	}
};

//==============================================================
// 문을 닫아 가두기

// 사람
class CDoorConfinementMan : public CMover {
public:
	float VX;

	CDoorConfinementMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;
		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		return true;
	}
};

// 문
class CDoorConfinementDoor : public CMover {
	CDoorConfinementMan* Man;
public:
	float OriginalX;
	int State;

	CDoorConfinementDoor(CDoorConfinementMan* man) {
		Texture=Game->Texture[TEX_DOOR2];
		Man=man;
		W=H=2;
		X=5;
		Y=MAX_Y-2.5f;
		OriginalX=X;
		State=0;
	}
	virtual bool Move(const CInputState* is) {
		float max_x=1.0f;
		float slide_width=1.2f;
		
		if (abs(Man->X-X)<max_x) X+=Man->VX;
		State=0;
		if (X>=OriginalX) {
			X=OriginalX;
			State=1;
		}
		if (X<=OriginalX-slide_width) {
			X=OriginalX-slide_width;
			State=2;
		}
		return true;
	}
	
	virtual void Draw() {
		float x=X;
		X=OriginalX;
		Texture=Game->Texture[TEX_DOOR3];
		CMover::Draw();

		X=x;
		Texture=Game->Texture[TEX_DOOR2];
		CMover::Draw();		
	}
};

// 적
class CDoorConfinementEnemy : public CMover {
	float VX;
	int State, Time;
	CDoorConfinementDoor* Door;
public:	
	CDoorConfinementEnemy(CDoorConfinementDoor* door) {
		Texture=Game->Texture[TEX_ENEMY0];
		X=MAX_X;
		Y=MAX_Y-2;
		VX=-Game->Rand.Real1()*0.1f-0.05f;
		State=0;
		Door=door;
		Color=COL_MGRAY;
		DrawOrder=1;
	}
	virtual bool Move(const CInputState* is) {
		float max_x=0.5f;
		float stay_time=60;
		
		switch (State) {
			case 0:
				if (
					Door->State==2 &&
					X-Door->OriginalX>0 && 
					X-Door->OriginalX<max_x
				) {
					Time=stay_time;
					State=1;
				}
				X+=VX;
				if (X<-1) X=MAX_X;
				break;
			case 1:
				if (Door->State==1) {
					X=MAX_X-1;
					State=0;
				} else {
					Time--;
					if (Time==0) {
						State=2;
					}
				}
				break;
			case 2:
				X+=VX;
				if (X<-1) {
					State=0;
					X=MAX_X;
				}
				break;
		}
		
		Angle=(State==1)?0.5f:0;
		return true;
	}
};

// 스테이지
class CDoorConfinementStage : public CStage {
	CDoorConfinementMan* Man;
	CDoorConfinementDoor* Door;
public:
	CDoorConfinementStage() : CStage("DOOR CONFINEMENT") {}
	virtual void Init() {
		Man=new CDoorConfinementMan();
		Door=new CDoorConfinementDoor(Man);
		for (int i=0; i<3; i++) new CDoorConfinementEnemy(Door);
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
	}
};

//==============================================================
// 엘리베이터

// 사람
class CElevatorMan : public CMover {
public:
	float VX;

	CElevatorMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float floor_max_x=0.8f;
		float floor_min_y=0.8f;
		float floor_max_y=1.0f;
		float elevator_max_x=0.8f;
		float elevator_min_y=-0.8f;
		float elevator_max_y=-0.3f;
		
		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;
		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		Y+=speed;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==2 &&
				abs(mover->X-X)<elevator_max_x &&
				mover->Y-Y>=elevator_min_y && 
				mover->Y-Y<=elevator_max_y
			) {
				Y=mover->Y-elevator_max_y;
				break;
			}
			if (
				mover->Type==1 &&
				abs(mover->X-X)<floor_max_x &&
				mover->Y-Y>=floor_min_y && 
				mover->Y-Y<=floor_max_y
			) {
				Y=mover->Y-floor_max_y;
				break;
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 엘리베이터
class CElevator : public CMover {
	int Time;
	float VY;
public:
	CElevator() {
		X=7.5f;
		Y=MAX_Y-2.5f;
		W=H=2;
		VY=-0.2f;
		Time=0;
		Type=2;
	}
	virtual bool Move(const CInputState* is) {
		int wait_time=120;
		float min_y=MAX_Y-2.5f-5;
		float max_y=MAX_Y-2.5f;
		
		if (Time>0) {
			Time--;
		} else {
			Y+=VY;
			if (Y<=min_y) {
				Y=min_y;
				Time=wait_time;
				VY=-VY;
			}
			if (Y>=max_y) {
				Y=max_y;
				Time=wait_time;
				VY=-VY;
			}
		}
		return true;
	}
	virtual void Draw() {
		float y=Y;
		Texture=Game->Texture[TEX_ELEVATOR1];
		for (Y-=H; Y+H>0; Y-=H) CMover::Draw();
		Y=y;
		Texture=Game->Texture[TEX_ELEVATOR0];
		CMover::Draw();
	}
};

// 스테이지
class CElevatorStage : public CStage {
	CElevatorMan* Man;
	CElevator* Elevator;
public:
	CElevatorStage() : CStage("ELEVATOR") {}
	virtual void Init() {
		Man=new CElevatorMan();
		Elevator=new CElevator();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1, 1);
		for (int i=0; i<7; i++) new CStaticFloor(i, 6, 1);
		for (int i=9; i<MAX_X; i++) new CStaticFloor(i, 6, 1);
	}
};

//==============================================================
// 움직이는 발판

// 움직이는 발판
class CMovingFloor : public CMover {
public:
	float OriginalX, VX, Range, Dir;
	int Time;
	CMovingFloor(float x, float y, int type, float range) {
		Texture=Game->Texture[TEX_MOVING_FLOOR];
		X=x;
		Y=y;
		VX=0;
		Dir=1;
		OriginalX=X;
		Range=range;
		Type=type;
		Time=0;
	}
	virtual bool Move(const CInputState* is) {
		float speed=0.1f;
		int wait_time=60;

		if (Time>0) {
			Time--;
		} else {
			VX=Dir*speed;
			X+=VX;
			if (
				X<=OriginalX ||
				X>=OriginalX+Range
			) {
				Time=wait_time;
				VX=0;
				Dir=-Dir;
			}
		}
		return true;
	}
};

// 사람
class CMovingFloorMan : public CMover {
public:
	float VX;

	CMovingFloorMan() {
		Texture=Game->Texture[TEX_MAN];
		X=1;
		Y=MAX_Y-2;
		VX=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_x=0.8f;
		float min_y=0.8f;
		float max_y=1.0f;
		
		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		Y+=speed;
		if (Y>MAX_Y) Y=-1;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type!=0 &&
				abs(mover->X-X)<max_x &&
				mover->Y-Y>=min_y && 
				mover->Y-Y<=max_y
			) {
				Y=mover->Y-max_y;
				if (mover->Type==2) {
					CMovingFloor* floor=(CMovingFloor*)mover;
					X+=floor->VX;
					break;
				}
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CMovingFloorStage : public CStage {
	CMovingFloorMan* Man;
public:
	CMovingFloorStage() : CStage("MOVING FLOOR") {}
	virtual void Init() {
		Man=new CMovingFloorMan();
		for (int i=0; i<4; i++) new CStaticFloor(i, MAX_Y-1, 1);
		for (int i=4; i<7; i++) new CMovingFloor(i, MAX_Y-1, 2, 5);
		for (int i=MAX_X-4; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1, 1);
	}
};

//==============================================================
// 컨베이어 벨트

// 컨베이어 벨트
class CConveyor : public CMover {
public:
	float VX;
	CConveyor(float x, float y, int type, int dir) {
		Texture=Game->Texture[TEX_CONVEYOR0];
		X=x;
		Y=y;
		VX=dir*0.1f;
		Type=type;
	}
};

// 컨베이어 벨트의 기어
class CConveyorGear : public CMover {
public:
	float VAngle;
	CConveyorGear(float x, float y, int dir) {
		Texture=Game->Texture[TEX_CONVEYOR1];
		X=x;
		Y=y;
		VAngle=dir*0.01f;
		DrawOrder=1;
	}
	virtual bool Move(const CInputState* is) {
		Angle+=VAngle;
		return true;
	}
};

// 사람
class CConveyorMan : public CMover {
public:
	float VX;

	CConveyorMan() {
		Texture=Game->Texture[TEX_MAN];
		X=1;
		Y=MAX_Y-2;
		VX=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_x=0.8f;
		float min_y=0.8f;
		float max_y=1.0f;
		
		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		Y+=speed;
		if (Y>MAX_Y) Y=-1;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type!=0 &&
				abs(mover->X-X)<max_x &&
				mover->Y-Y>=min_y && 
				mover->Y-Y<=max_y
			) {
				Y=mover->Y-max_y;
				if (mover->Type==2) {
					CConveyor* conveyor=(CConveyor*)mover;
					X+=conveyor->VX;
					break;
				}
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CConveyorStage : public CStage {
	CConveyorMan* Man;
public:
	CConveyorStage() : CStage("BELT CONVEYOR") {}
	virtual void Init() {
		Man=new CConveyorMan();
		for (int i=0; i<2; i++) new CStaticFloor(i, MAX_Y-1, 1);
		for (int i=2; i<7; i++) new CConveyor(i, MAX_Y-1, 2, 1);
		for (int i=7; i<9; i++) new CStaticFloor(i, MAX_Y-1, 1);
		for (int i=9; i<14; i++) new CConveyor(i, MAX_Y-1, 2, -1);
		for (int i=14; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1, 1);
		new CConveyorGear(2, MAX_Y-1, 1);
		new CConveyorGear(6, MAX_Y-1, 1);
		new CConveyorGear(9, MAX_Y-1, -1);
		new CConveyorGear(13, MAX_Y-1, -1);
	}
};

//==============================================================
// 상승 기류

// 기류
class CAirCurrent : public CMover {
public:
	CAirCurrent(float x, float y) {
		Texture=Game->Texture[TEX_AIR_CURRENT1];
		X=x;
		Y=y;
		Color=COL_MGRAY;
	}
	virtual bool Move(const CInputState* is) {
		Y-=0.1f;
		return Y>=-1;
	}
};

// 팬
class CAirCurrentFan : public CMover {
public:
	int Time;
	float VAngle;
	CAirCurrentFan(float x, float y) {
		Texture=Game->Texture[TEX_AIR_CURRENT0];
		X=x;
		Y=y;
		Type=1;
		Time=0;
	}
	virtual bool Move(const CInputState* is) {
		int wait_time=40;
		Angle+=0.01f;
		if (Time>0) {
			Time--;
		} else {
			new CAirCurrent(X, Y);
			Time=wait_time;
		}
		return true;
	}
};

// 사람
class CAirCurrentMan : public CMover {
public:
	float VX;

	CAirCurrentMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=3;
		VX=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float up_speed=-0.05f;
		float down_speed=0.03f;
		float max_x=0.6f;
		
		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		float vy=down_speed;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(mover->X-X)<max_x
			) {
				vy=up_speed;
				break;
			}
		}
		Y+=vy;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CAirCurrentStage : public CStage {
	CAirCurrentMan* Man;
public:
	CAirCurrentStage() : CStage("AIR CURRENT") {}
	virtual void Init() {
		Man=new CAirCurrentMan();
		for (int i=0; i<4; i++) new CAirCurrentFan(i*3+3, MAX_Y-1);
	}
};

//==============================================================
// 워프 게이트

// 워프 게이트
class CWarpGate : public CMover {
public:
	float DestX;
	CWarpGate(float x, float y, int type, float dest_x) {
		Texture=Game->Texture[TEX_WARP_GATE];
		X=x;
		Y=y;
		W=2;
		DestX=dest_x;
		Type=type;
		DrawOrder=1;
	}
};

// 사람
class CWarpGateMan : public CMover {
public:
	float VX, DestX;
	int State;

	CWarpGateMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		State=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.1f;
		float warp_speed=0.1f;
		float max_x=0.5f;
		
		switch (State) {
			case 0:
				VX=0;
				if (is->Left) VX=-speed;
				if (is->Right) VX=speed;
				X+=VX;
				if (X<0) X=0;
				if (X>MAX_X-1) X=MAX_X-1;
				
				if (is->Down) {
					for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
						CMover* mover=(CMover*)i.Next();
						if (
							mover->Type==1 &&
							abs(mover->X-X)<max_x
						) {
							CWarpGate* gate=(CWarpGate*)mover;
							DestX=gate->DestX;
							State=1;
							break;
						}
					}
				}
				break;
			case 1:
				Y+=warp_speed;
				if (Y>=MAX_Y-1) {
					X=DestX;
					State=2;
				} 
				break;
			case 2:
				Y-=warp_speed;
				if (Y<=MAX_Y-2) State=0;
				break;
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CWarpGateStage : public CStage {
	CWarpGateMan* Man;
public:
	CWarpGateStage() : CStage("WARP GATE") {}
	virtual void Init() {
		Man=new CWarpGateMan();
		for (int i=0; i<8; i++) new CWarpGate(i*2+0.5f, MAX_Y-1, 1, ((i+5)%8)*2+0.5f);
	}
};

//==============================================================
// 형태가 바뀌는 통로

// 형태가 바뀌는 통로
class CSwitchingPath : public CMover {
public:
	float Left, Right, VX;
	bool Switching;
	CSwitchingPath(float x, float y, float left, float right) {
		Texture=Game->Texture[TEX_LADDER];
		X=x;
		Y=y;
		Left=left;
		Right=right;
		Type=2;
		Switching=false;
	}
	void Switch() {
		if (!Switching) {
			float speed=0.1f;
			if (X>Left) VX=-speed; else VX=speed;
			Switching=true;
		}
	}
	virtual bool Move(const CInputState* is) {
		if (Switching) {
			X+=VX;
			if (X<=Left || X>=Right) Switching=false;
		}
		return true;
	}
};

// 사람
class CSwitchingPathMan : public CMover {
public:
	float VX, VY;

	CSwitchingPathMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=3;
		VX=0;
		VY=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float ground_y=MAX_Y-2;
		float ladder_max_x=0.6f;
		float ladder_min_y=-1.0f;
		float ladder_max_y=1.0f;
		float floor_max_x=0.8f;
		float floor_min_y=0.6f;
		float floor_max_y=1.0f;
		
		bool ladder=false;
		bool floor=false;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==2 &&
				abs(mover->X-X)<ladder_max_x &&
				mover->Y-Y>=ladder_min_y && 
				mover->Y-Y<=ladder_max_y
			) {
				if (mover->Y-Y<ladder_max_y-speed) ladder=true; else floor=true;
				break;
			}
		}
		
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(mover->X-X)<floor_max_x &&
				mover->Y-Y>=floor_min_y && 
				mover->Y-Y<=floor_max_y
			) {
				floor=true;
				break;
			}
		}

		if (ladder || floor) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			X+=VX;
			if (X<0) X=0;
			if (X>MAX_X-1) X=MAX_X-1;

			VY=0;
			if (ladder && is->Up) VY=-speed;
			if (is->Down) VY=speed;		
			Y+=VY;
			if (Y<0) Y=0;
			if (Y>ground_y) Y=ground_y;

			if (ladder && is->Button[0]) {
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (mover->Type==2) {
						CSwitchingPath* ladder=(CSwitchingPath*)mover;
						ladder->Switch();
					}
				}
			}
		}
		
		if (!ladder && !floor) {
			Y+=speed;
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CSwitchingPathStage : public CStage {
	CSwitchingPathMan* Man;
public:
	CSwitchingPathStage() : CStage("SWITCHING PATH") {}
	virtual void Init() {
		Man=new CSwitchingPathMan();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1, 1);
		for (int i=0; i<6; i++) new CStaticFloor(i, MAX_Y-5, 1);
		for (int i=10; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-5, 1);
		new CSwitchingPath(6, MAX_Y-2, 6, 9);
		new CSwitchingPath(7, MAX_Y-3, 7, 8);
		new CSwitchingPath(8, MAX_Y-4, 7, 8);
		new CSwitchingPath(9, MAX_Y-5, 6, 9);
	}
};

//==============================================================
// 벽을 파괴하여 통로 만들기

// 사람
class CBreakWallMan : public CMover {
public:
	float VX, VY;

	CBreakWallMan() {
		Texture=Game->Texture[TEX_MAN];
		X=7;
		Y=5;
		VX=0;
		VY=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float walk_diff=1.0f;
		float break_dist=2.1f;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;
		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		VY=0;
		if (is->Up) VY=-speed;
		if (is->Down) VY=speed;
		Y+=VY;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;
		
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(mover->X-X)<walk_diff &&
				abs(mover->Y-Y)<walk_diff
			) {
				X-=VX;
				Y-=VY;
				break;
			}
		}
		
		if (is->Button[0]) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (mover->Type==1) {
					float dx=mover->X-X;
					float dy=mover->Y-Y;
					if (sqrt(dx*dx+dy*dy)<break_dist) {
						i.Remove();
					}
				}
			}
		}
				
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CBreakWallStage : public CStage {
	CBreakWallMan* Man;
public:
	CBreakWallStage() : CStage("BREAKING WALL") {}
	virtual void Init() {
		Man=new CBreakWallMan();
		for (int j=2; j<MAX_Y; j++) {
			for (int i=0; i<MAX_X; i++) {
				if (i!=Man->X || j!=Man->Y) new CStaticFloor(i, j, 1);
			}
		}
	}
};

