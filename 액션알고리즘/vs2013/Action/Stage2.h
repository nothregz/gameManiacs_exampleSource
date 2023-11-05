//==============================================================
// 고정 길이 점프

// 사람
class CFixedJumpMan : public CMover {
public:
	float VX, VY;
	bool Jump;

	CFixedJumpMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		Jump=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.15f;
		float jump_speed=-0.5f;
		float jump_accel=0.02f;
		float ground_y=MAX_Y-2;

		if (!Jump) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0]) {
				Jump=true;
				VY=jump_speed;
			}
		} else
		{
			VY+=jump_accel;
			Y+=VY;
			if (VY>0 && Y>=ground_y) {
				Jump=false;
				Y=ground_y;
			}
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CFixedJumpStage : public CStage {
	CFixedJumpMan* Man;
public:
	CFixedJumpStage() : CStage("FIXED JUMP") {}
	virtual void Init() {
		Man=new CFixedJumpMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 가변 길이 점프

// 사람
class CVariableJumpMan : public CMover {
public:
	float VX, VY;
	bool Jump;

	CVariableJumpMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		Jump=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.15f;
		float jump_speed=-0.5f;
		float jump_accel=0.02f;
		float ground_y=MAX_Y-2;

		if (!Jump) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0]) {
				Jump=true;
				VY=jump_speed;
			}
		} else
		{
			VY+=jump_accel;
			Y+=VY;
			if (VY>0 && Y>=ground_y) {
				Jump=false;
				Y=ground_y;
			}
			if (VY<0 && !is->Button[0]) {
				VY=0;
			}
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CVariableJumpStage : public CStage {
	CVariableJumpMan* Man;
public:
	CVariableJumpStage() : CStage("VARIABLE JUMP") {}
	virtual void Init() {
		Man=new CVariableJumpMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 2단 점프

// 사람
class CDoubleJumpMan : public CMover {
public:
	float VX, VY;
	int JumpCount;
	bool PrevButton;

	CDoubleJumpMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		JumpCount=0;
		PrevButton=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.15f;
		float jump_speed=-0.4f;
		float jump_accel=0.02f;
		float ground_y=MAX_Y-2;
		float double_jump_vy=0.1f;
		int jump_count=2;

		if (JumpCount==0) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0]) {
				JumpCount=1;
				VY=jump_speed;
			}
		} else
		{
			VY+=jump_accel;
			Y+=VY;
			if (VY>0 && Y>=ground_y) {
				JumpCount=0;
				Y=ground_y;
			}
			if (abs(VY)<double_jump_vy && !PrevButton && is->Button[0] && JumpCount<jump_count) {
				VY=jump_speed;
				JumpCount++;
			}
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CDoubleJumpStage : public CStage {
	CDoubleJumpMan* Man;
public:
	CDoubleJumpStage() : CStage("DOUBLE JUMP") {}
	virtual void Init() {
		Man=new CDoubleJumpMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 삼각 점프

// 사람
class CTriangleJumpMan : public CMover {
public:
	float VX, VY;
	int JumpCount;
	bool PrevButton;

	CTriangleJumpMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		JumpCount=0;
		PrevButton=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.15f;
		float jump_speed=-0.4f;
		float jump_accel=0.02f;
		float ground_y=MAX_Y-2;
		float wall_distance=0.1f;
		int jump_count=2;

		if (JumpCount==0) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0]) {
				JumpCount=1;
				VY=jump_speed;
			}
		} else
		{
			VY+=jump_accel;
			Y+=VY;
			if (VY>0 && Y>=ground_y) {
				JumpCount=0;
				Y=ground_y;
			}
			if (
				(VX<0 && X<wall_distance || VX>0 && MAX_X-1-X<wall_distance) &&
				!PrevButton && is->Button[0] && 
				JumpCount<jump_count
			) {
				VX=-VX;
				VY=jump_speed;
				JumpCount++;
			}
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CTriangleJumpStage : public CStage {
	CTriangleJumpMan* Man;
public:
	CTriangleJumpStage() : CStage("TRIANGLE JUMP") {}
	virtual void Init() {
		Man=new CTriangleJumpMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 뛰어내리기

// 사람
class CJumpOffMan : public CMover {
public:
	float VX, VY;
	bool Jump;

	CJumpOffMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		Jump=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.15f;
		float jump_speed=-0.4f;
		float jump_accel=0.02f;
		float bottom_y=MAX_Y-2;
		float floor_max_x=0.8f;
		float floor_min_y=1.0f;
		float floor_max_y=1.4f;

		if (!Jump) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0] && !is->Down) {
				Jump=true;
				VY=jump_speed;
			}
			if (is->Button[0] && is->Down && Y<bottom_y) {
				Jump=true;
				VY=0;
			}

			bool on_floor=false;
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 && 
					abs(mover->X-X)<floor_max_x && 
					mover->Y-Y==floor_min_y
				) {
					on_floor=true;
					break;
				}
			}
			if (Y<bottom_y && !on_floor) {
				Jump=true;
				VY=0;
			}
		} else
		{
			VY+=jump_accel;
			if (VY>floor_max_y-floor_min_y) {
				VY=floor_max_y-floor_min_y;
			}
			Y+=VY;

			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					VY>0 &&
					mover->Type==1 && 
					abs(mover->X-X)<floor_max_x && 
					mover->Y-Y>floor_min_y &&
					mover->Y-Y<=floor_max_y
				) {
					Jump=false;
					Y=mover->Y-floor_min_y;
					break;
				}
			}
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 플랫폼
class CJumpOffFloor : public CMover {
public:
	CJumpOffFloor(float x, float y) {
		Texture=Game->Texture[TEX_FLOOR];
		X=x;
		Y=y;
		Type=1;
	}
};

// 스테이지
class CJumpOffStage : public CStage {
	CJumpOffMan* Man;
public:
	CJumpOffStage() : CStage("JUMP OFF") {}
	virtual void Init() {
		Man=new CJumpOffMan();
		for (int j=0; j<4; j++) {
			for (int i=-1+j*2; i<MAX_X+1-j*2; i++) {
				new CJumpOffFloor(i, MAX_Y-1-j*3);
			}
		}
	}
};


//==============================================================
// 점프 비행

// 사람
class CJumpFlyMan : public CMover {
public:
	float VX, VY;

	CJumpFlyMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.15f;
		float jump_speed=-0.3f;
		float jump_accel=0.03f;
		float ground_y=MAX_Y-2;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;
		if (is->Button[0]) VY=jump_speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		VY+=jump_accel;
		if (VY>-jump_speed) VY=-jump_speed;
		
		Y+=VY;
		if (Y<0) Y=0;

		if (VY>0 && Y>=ground_y) {
			VY=0;
			Y=ground_y;
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CJumpFlyStage : public CStage {
	CJumpFlyMan* Man;
public:
	CJumpFlyStage() : CStage("UNIVERSAL JUMP") {}
	virtual void Init() {
		Man=new CJumpFlyMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
	}
};


//==============================================================
// 점프 각도 조절

// 사람
class CJumpAngleMan : public CMover {
public:
	float VX, VY;
	bool PrevButton;
	float JumpSpeed, JumpAngle, JumpDist;
	int JumpState;

	CJumpAngleMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		PrevButton=false;
		JumpDist=0;
		JumpState=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float run_speed=0.6f;
		float run_accel=0.06f;
		float jump_speed=-0.5f;
		float jump_accel=0.02f;
		float jump_angle_max=0.25f;
		float jump_angle_add=0.005f;
		float ground_y=MAX_Y-2;

		switch (JumpState) {
			case 0:
				if (!PrevButton && is->Button[1]) {
					VX+=run_accel;
				} else {
					VX-=run_accel*0.05f;
				}
				if (VX<0) VX=0;
				if (VX>run_speed) VX=run_speed;		
				PrevButton=is->Button[1];

				if (is->Button[0]) {
					JumpSpeed=sqrtf(VX*VX+jump_speed*jump_speed);
					VX=0;
					JumpAngle=0;
					JumpDist=0;
					JumpState=1;
				}
				break;
			case 1:
				if (!is->Button[0] || JumpAngle==jump_angle_max) {
					VX=JumpSpeed*cosf(JumpAngle*D3DX_PI*2);
					VY=-JumpSpeed*sinf(JumpAngle*D3DX_PI*2);
					JumpState=2;
				} else {
					JumpAngle+=jump_angle_add;
					if (JumpAngle>jump_angle_max) JumpAngle=jump_angle_max;
				}
				break;
			case 2:
				JumpDist+=VX;
				VY+=jump_accel;
				Y+=VY;
				if (VY>0 && Y>=ground_y) {
					VX=0;
					Y=ground_y;
					JumpState=0;
				}
				break;
		}

		if (JumpState==0) Angle=VX/run_speed*0.1f; else Angle=-JumpAngle;
		return true;
	}
};

// 스테이지
class CJumpAngleStage : public CStage {
	CJumpAngleMan* Man;
public:
	CJumpAngleStage() : CStage("JUMP ANGLE") {}
	virtual void Init() {
		Man=new CJumpAngleMan();
		for (int i=0; i<MAX_X+1; i++) new CStaticFloor(i);
	}
	virtual bool Move(const CInputState* is) {
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover!=Man) mover->X-=Man->VX;
		}
		return true;
	}
	virtual void Draw() {
		float sw=Game->GetGraphics()->GetWidth();
		float sh=Game->GetGraphics()->GetHeight();
		char buf[100];
		sprintf_s(buf, sizeof(buf), "DISTANCE %f", Man->JumpDist);
		Game->Font->DrawText(buf, 0, sh*1.5f/MAX_Y, COL_BLACK);
	}
};


//==============================================================
// 플랫폼 어택

// 사람
class CFloorAttackMan : public CMover {
public:
	float VX, VY;
	bool Jump;

	CFloorAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.15f;
		float jump_speed=-0.5f;
		float jump_accel=0.02f;
		float ground_y=MAX_Y-2;

		if (!Jump) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0]) {
				Jump=true;
				VY=jump_speed;
			}
		} else
		{
			VY+=jump_accel;
			Y+=VY;
			if (VY>0 && Y>=ground_y) {
				Jump=false;
				Y=ground_y;
			}
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 플랫폼
class CFloorAttackFloor : public CMover {
public:
	CFloorAttackMan* Man;
	float OriginalY, VY;
	bool Attack;

	CFloorAttackFloor(float x, float y, CFloorAttackMan* man) {
		Texture=Game->Texture[TEX_FLOOR];
		X=x;
		Y=y;
		OriginalY=y;
		Man=man;
		Attack=false;
		Type=1;
	}
	virtual bool Move(const CInputState* is) {
		float max_x=0.8f;
		float min_y=0.4f;
		float max_y=1.0f;
		float attack_speed=-0.2f;
		float attack_accel=0.02f;

		if (!Attack) {
			if (
				Man->VY<=0 &&
				abs(Man->X-X)<max_x &&
				Man->Y-Y>min_y && Man->Y-Y<max_y
			) {
				Man->VY=0;
				VY=attack_speed;
				Attack=true;
			}
		} else {
			VY+=attack_accel;
			Y+=VY;
			if (Y>=OriginalY) {
				Y=OriginalY;
				Attack=false;
			}
		}
		return true;
	}
};

// 적
class CFloorAttackEnemy : public CMover {
public:
	float VX, VY, OriginalY;
	int Time;
	bool Sleep, Jump;
	
	CFloorAttackEnemy(float y) {
		Texture=Game->Texture[TEX_ENEMY0];
		X=Game->Rand.Real1()*(MAX_X-1);
		Y=y;
		OriginalY=Y;
		VX=Game->Rand.Real1()*0.2f+0.1f;
		Sleep=false;
		Jump=false;
	}
	virtual bool Move(const CInputState* is) {
		float max_x=0.8f;
		float min_y=0.4f;
		float max_y=1.0f;
		float jump_speed=-0.3f;
		float jump_accel=0.03f;
		int sleep_time=60;

		if (!Jump) {
			if (!Sleep) {
				X+=VX;
				if (X>MAX_X) X=-1;
			} else {
				Time++;
				if (Time>=sleep_time) Sleep=false;
			}
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (mover->Type==1) {
					CFloorAttackFloor* floor=(CFloorAttackFloor*)mover;
					if (
						abs(floor->X-X)<max_x && 
						floor->Y-Y>min_y && floor->Y-Y<max_y
					) {
						VY=jump_speed;
						Sleep=!Sleep;
						Time=0;
						Jump=true;
					}
				}
			}
		} else {
			if (Sleep) {
				X+=VX;
				if (X>MAX_X) X=-1;
			}
			VY+=jump_accel;
			Y+=VY;
			if (Y>=OriginalY) {
				Y=OriginalY;
				Jump=false;
			}
		}
		
		Angle=Sleep?0.5f:0;
		return true;
	}
};

// 스테이지
class CFloorAttackStage : public CStage {
	CFloorAttackMan* Man;
public:
	CFloorAttackStage() : CStage("FLOOR ATTACK") {}
	virtual void Init() {
		float floor_height=5;
		Man=new CFloorAttackMan();
		for (int j=0; j<2; j++) {
			for (int i=0; i<MAX_X; i++) {
				new CFloorAttackFloor(i, MAX_Y-1-j*floor_height, Man);
			}
		}
		for (int i=0; i<5; i++) {
			new CFloorAttackEnemy(MAX_Y-2-floor_height);
		}
	}
};


//==============================================================
// 밟기

// 사람
class CStompMan : public CMover {
public:
	float VX, VY;
	bool Jump;

	CStompMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.15f;
		float jump_speed=-0.5f;
		float jump_accel=0.02f;
		float ground_y=MAX_Y-2;

		if (!Jump) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0]) {
				Jump=true;
				VY=jump_speed;
			}
		} else
		{
			VY+=jump_accel;
			Y+=VY;
			if (VY>0 && Y>=ground_y) {
				Jump=false;
				Y=ground_y;
			}
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 적
class CStompEnemy : public CMover {
public:
	float VX, VY;
	bool Stomped;
	CStompMan* Man;
	
	CStompEnemy(CStompMan* man) {
		Texture=Game->Texture[TEX_ENEMY1];
		X=Game->Rand.Real1()*(MAX_X-1);
		Y=MAX_Y-2;
		VX=Game->Rand.Real1()*0.1f+0.05f;
		Man=man;
		Stomped=false;
	}

	virtual bool Move(const CInputState* is) {
		float max_x=0.8f;
		float min_y=0.1f;
		float max_y=1.0f;
		float smash_speed=-0.1f;
		
		if (!Stomped) {
			X+=VX;
			if (X>MAX_X) X=-1;
			if (
				Man->VY>0 &&
				abs(X-Man->X)<max_x &&
				Y-Man->Y>min_y && Y-Man->Y<max_y
			) {
				Stomped=true;
			}
		} else {
			H+=smash_speed;
			Y-=smash_speed*0.5f;
			if (H<0) {
				Stomped=false;
				H=1;
				Y=MAX_Y-2;
				X=-1;
			}
		}
		return true;
	}
};

// 스테이지
class CStompStage : public CStage {
	CStompMan* Man;
public:
	CStompStage() : CStage("STOMP") {}
	virtual void Init() {
		Man=new CStompMan();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i);
		for (int i=0; i<10; i++) new CStompEnemy(Man);
	}
};


//==============================================================
// 스프링보드

// 사람
class CStampBoardMan : public CMover {
public:
	float VX, VY;
	bool Jump, PrevButton;

	CStampBoardMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		PrevButton=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float jump_speed=-0.3f;
		float jump_accel=0.02f;
		float ground_y=MAX_Y-2;
		float board_jump_speed=-0.6f;
		float max_x=1.0f;
		float min_y=-1.0f;
		float max_y=0.8f;

		if (!Jump) {
			VX=0;
			if (is->Right) VX=speed;
			if (is->Button[0]) {
				Jump=true;
				VY=jump_speed;
			}
		} else
		{
			VY+=jump_accel;
			Y+=VY;
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					VY>0 &&
					!PrevButton && is->Button[0] &&
					abs(mover->X-X)<max_x && 
					mover->Y-Y>min_y && mover->Y-Y<max_y
				) {
					VY=board_jump_speed;
				}
			}
			if (VY>0 && Y>=ground_y) {
				Jump=false;
				Y=ground_y;
			}
		}
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스프링보드
class CStampBoard : public CMover {
public:
	CStampBoard() {
		Texture=Game->Texture[TEX_STAMP_BOARD];
		W=2;
		X=MAX_X+W;
		Y=MAX_Y-2;
		Type=1;
	}
	virtual bool Move(const CInputState* is) {
		return X>-W;
	}
};

// 스테이지
class CStampBoardStage : public CStage {
	CStampBoardMan* Man;
	float Dist;
public:
	CStampBoardStage() : CStage("STAMP BOARD") {}
	virtual void Init() {
		Man=new CStampBoardMan();
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
			new CStampBoard();
			Dist=Game->Rand.Real1()*MAX_X*2;
		}
		return true;
	}
};

