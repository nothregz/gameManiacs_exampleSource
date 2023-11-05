// 적
class CAttackEnemy : public CMover {
public:
	float VX, VY;
	int State;

	void Init() {
		X=MAX_X;
		Y=MAX_Y-2;
		VX=-Game->Rand.Real1()*0.05f-0.05f;
		Angle=0;
		State=0;
	}

	CAttackEnemy() {
		Texture=Game->Texture[TEX_ENEMY1];
		Type=1;
		Init();
	}

	void Attack() {
		float vx=0.4f;
		float vy=-0.5f;
		if (State==0) {
			VX=vx;
			VY=vy;
			State=1;
		}
	}

	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		float vangle=0.05f;
		switch (State) {
			case 0:
				X+=VX;
				if (X<-1) Init();
				break;
			case 1:
				X+=VX;
				VY+=accel;
				Y+=VY;
				Angle+=vangle;
				if (X>MAX_X) Init();
				break;
		}
		return true;
	}
};


//==============================================================
// 몸으로 부딪치는 공격

// 사람
class CBodyAttackMan : public CMover {
public:
	float VX;

	CBodyAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.6f;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(X-mover->X)<max_dist
			) {
				CAttackEnemy* enemy=(CAttackEnemy*)mover;
				enemy->Attack();
			}
		}
		
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CBodyAttackStage : public CStage {
public:
	CBodyAttackStage() : CStage("BODY ATTACK") {}
	virtual void Init() {
		new CBodyAttackMan();
		for (int i=0; i<3; i++) new CAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 점프 공격

// 적
class CJumpAttackEnemy : public CMover {
public:
	float VX, VY;
	int State;

	void Init() {
		X=MAX_X;
		Y=MAX_Y-2-Game->Rand.Real1()*4;
		VX=-Game->Rand.Real1()*0.05f-0.05f;
		Angle=0;
		State=0;
	}

	CJumpAttackEnemy() {
		Texture=Game->Texture[TEX_ENEMY0];
		Type=1;
		Init();
	}

	void Attack() {
		float vx=0.4f;
		float vy=-0.5f;
		if (State==0) {
			VX=vx;
			VY=vy;
			State=1;
		}
	}

	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		float vangle=0.05f;
		switch (State) {
			case 0:
				X+=VX;
				if (X<-1) Init();
				break;
			case 1:
				X+=VX;
				VY+=accel;
				Y+=VY;
				Angle+=vangle;
				if (X>MAX_X) Init();
				break;
		}
		return true;
	}
};

// 사람
class CJumpAttackMan : public CMover {
public:
	float VX, VY;
	bool Jump;

	CJumpAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=VY=0;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float jump_speed=-0.4f;
		float jump_accel=0.02f;
		float max_dist=0.6f;

		if (!Jump) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0]) {
				VY=jump_speed;
				Jump=true;
			}
		} else {
			if (is->Button[1]) {
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==1 &&
						abs(X-mover->X)<max_dist &&
						abs(Y-mover->Y)<max_dist
					) {
						CJumpAttackEnemy* enemy=(CJumpAttackEnemy*)mover;
						enemy->Attack();
					}
				}
			}
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;
		
		VY+=jump_accel;
		Y+=VY;
		if (Y>=MAX_Y-2) {
			Y=MAX_Y-2;
			Jump=false;
		}

		Angle=VX/speed*0.1f;
		if (Jump && is->Button[1]) Angle=(VX<0?0.25f:-0.25f);
		return true;
	}
};

// 스테이지
class CJumpAttackStage : public CStage {
public:
	CJumpAttackStage() : CStage("JUMP ATTACK") {}
	virtual void Init() {
		new CJumpAttackMan();
		for (int i=0; i<3; i++) new CJumpAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 잡기 공격

// 적
class CGrabAttackEnemy : public CMover {
public:
	float VX, VY;
	int State;

	void Init() {
		X=MAX_X;
		Y=MAX_Y-2;
		VX=-Game->Rand.Real1()*0.05f-0.05f;
		Angle=0;
		State=0;
	}

	CGrabAttackEnemy() {
		Texture=Game->Texture[TEX_ENEMY1];
		Type=1;
		Init();
	}

	void Grab() {
		if (State==0) State=1;
	}

	void Attack() {
		float vx=0.4f;
		float vy=-0.5f;
		if (State==1) {
			VX=vx;
			VY=vy;
			State=2;
		}
	}

	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		float vangle=0.05f;
		switch (State) {
			case 0:
				X+=VX;
				if (X<-1) Init();
				break;
			case 1:
				Angle=0.25f;
				break;
			case 2:
				X+=VX;
				VY+=accel;
				Y+=VY;
				Angle+=vangle;
				if (X>MAX_X) Init();
				break;
		}
		return true;
	}
};

// 사람
class CGrabAttackMan : public CMover {
public:
	float VX;
	CGrabAttackEnemy* Enemy;

	CGrabAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		Enemy=NULL;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.6f;

		if (!Enemy) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;

			X+=VX;
			if (X<0) X=0;
			if (X>MAX_X-1) X=MAX_X-1;

			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					abs(X-mover->X)<max_dist
				) {
					Enemy=(CGrabAttackEnemy*)mover;
					Enemy->Grab();
					break;
				}
			}
		} else {
			if (is->Button[0]) {
				Enemy->Attack();
				Enemy=NULL;
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CGrabAttackStage : public CStage {
public:
	CGrabAttackStage() : CStage("GRAB ATTACK") {}
	virtual void Init() {
		new CGrabAttackMan();
		for (int i=0; i<3; i++) new CGrabAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 정지 공격

// 적
class CStickingAttackEnemy : public CMover {
public:
	float VX;
	int State, Time;

	void Init() {
		X=MAX_X;
		Y=MAX_Y-2;
		VX=-Game->Rand.Real1()*0.05f-0.05f;
		Angle=0;
		State=0;
	}

	CStickingAttackEnemy() {
		Texture=Game->Texture[TEX_ENEMY0];
		Type=1;
		Init();
	}

	void Attack() {
		int wait_time=90;
		if (State==0) {
			Time=wait_time;
			State=1;
		}
	}

	virtual bool Move(const CInputState* is) {
		float vangle=0.05f;
		float max_angle=4;
		switch (State) {
			case 0:
				X+=VX;
				if (X<-1) Init();
				break;
			case 1:
				Angle+=vangle;
				Time--;
				if (Time==0) {
					Angle=0;
					State=0;
				}
				break;
		}
		return true;
	}
};

// 사람
class CStickingAttackMan : public CMover {
public:
	float VX, DirX;
	bool Attack;

	CStickingAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		Attack=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.6f;

		VX=0;
		if (is->Left) {
			DirX=-1;
			VX=-speed;
		}
		if (is->Right) {
			DirX=1;
			VX=speed;
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Attack=false;
		if (is->Button[0]) {
			Attack=true;
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					abs(X+DirX-mover->X)<max_dist
				) {
					CStickingAttackEnemy* enemy=(CStickingAttackEnemy*)mover;
					enemy->Attack();
				}
			}
		}
		
		Angle=VX/speed*0.1f;
		return true;
	}

	virtual void Draw() {
		float x=X;
		if (Attack) {
			X=x+DirX*0.75f;
			ReverseX=(DirX<0);
			Texture=Game->Texture[TEX_PEPPER];
			CMover::Draw();
		}
		Texture=Game->Texture[TEX_MAN];
		X=x;
		ReverseX=false;
		CMover::Draw();
	}
};

// 스테이지
class CStickingAttackStage : public CStage {
public:
	CStickingAttackStage() : CStage("STICKING ATTACK") {}
	virtual void Init() {
		new CStickingAttackMan();
		for (int i=0; i<3; i++) new CStickingAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 함정

// 함정
class CTrap : public CMover {
public:
	CTrap(float x, float y) {
		Texture=Game->Texture[TEX_BOMB];
		X=x;
		Y=y;
	}
	virtual bool Move(const CInputState* is) {
		float max_dist=0.6f;
		bool attacked=false;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(X-mover->X)<max_dist
			) {
				CAttackEnemy* enemy=(CAttackEnemy*)mover;
				enemy->Attack();
				attacked=true;
			}
		}
		return !attacked;
	}
};

// 사람
class CTrapMan : public CMover {
public:
	float VX;
	bool PrevButton;

	CTrapMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		if (!PrevButton && is->Button[0]) new CTrap(X, Y);
		PrevButton=is->Button[0];
		
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CTrapStage : public CStage {
public:
	CTrapStage() : CStage("TRAP") {}
	virtual void Init() {
		new CTrapMan();
		for (int i=0; i<3; i++) new CAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 전 범위 공격

// 사람
class CAllRangeAttackMan : public CMover {
public:
	float VX;
	bool PrevButton;

	CAllRangeAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		if (!PrevButton && is->Button[0]) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (mover->Type==1) {
					CAttackEnemy* enemy=(CAttackEnemy*)mover;
					enemy->Attack();
				}
			}
		}
		PrevButton=is->Button[0];
		
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CAllRangeAttackStage : public CStage {
public:
	CAllRangeAttackStage() : CStage("TOTAL RANGE ATTACK") {}
	virtual void Init() {
		new CAllRangeAttackMan();
		for (int i=0; i<3; i++) new CAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 차지 공격

// 사람
class CChargeAttackMan : public CMover {
public:
	float VX, DirX, Charge;
	int State, Time;

	CChargeAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		State=0;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.1f;
		float max_dist=0.6f;
		float charge_speed=0.05f;
		float max_charge=4;
		int attack_time=10;

		switch (State) {
			case 0:
				VX=0;
				if (is->Left) {
					DirX=-1;
					VX=-speed;
				}
				if (is->Right) {
					DirX=1;
					VX=speed;
				}
				if (is->Button[0]) {
					Charge=0;
					State=1;
				}
				X+=VX;
				if (X<0) X=0;
				if (X>MAX_X-1) X=MAX_X-1;
				Angle=VX/speed*0.1f;
				break;
			
			case 1:
				Charge+=charge_speed;
				if (Charge>max_charge) Charge=max_charge;
				W=H=Charge+1;
				Y=MAX_Y-2-Charge/2;
				if (!is->Button[0]) {
					Time=attack_time*Charge;
					State=2;
				}
				Angle=0;
				break;
			
			case 2:
				X+=DirX*speed*Charge;
				if (X<0) X=0;
				if (X>MAX_X-1) X=MAX_X-1;
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==1 &&
						abs(X-mover->X)<max_dist*(Charge+1)
					) {
						CAttackEnemy* enemy=(CAttackEnemy*)mover;
						enemy->Attack();
					}
				}
				Time--;
				if (Time==0) State=3;
				Angle=DirX*0.1f;
				break;

			case 3:
				Charge-=charge_speed*4;
				W=H=Charge+1;
				Y=MAX_Y-2-Charge/2;
				if (Charge<=0) {
					W=H=1;
					Y=MAX_Y-2;
					State=0;
				}
				Angle=0;
				break;
		}

		return true;
	}
};

// 스테이지
class CChargeAttackStage : public CStage {
public:
	CChargeAttackStage() : CStage("CHARGE ATTACK") {}
	virtual void Init() {
		new CChargeAttackMan();
		for (int i=0; i<3; i++) new CAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 들이받기 공격

// 적
class CImpactAttackEnemy : public CMover {
public:
	float VX, VY;

	void Init() {
		X=-1;
		VX=Game->Rand.Real1()*0.05f+0.05f;
		if (Game->Rand.Int31()%2==0) {
			X=MAX_X;
			VX=-VX;
		}
		Y=Game->Rand.Real1()*(MAX_Y-1);
		VY=0;
	}

	CImpactAttackEnemy() {
		Texture=Game->Texture[TEX_ENEMY0];
		Type=1;
		Init();
	}

	virtual bool Move(const CInputState* is) {
		X+=VX;
		Y+=VY;
		if (X<-1 || X>MAX_X || Y<-1 || Y>MAX_Y) Init();
		return true;
	}
};

// 사람
class CImpactAttackMan : public CMover {
public:
	float VX, VY;
	int Time;

	CImpactAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=(MAX_Y-1)/2;
		VX=VY=0;
		Time=0;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.6f;
		int impact_time=10;

		if (Time==0) {
			VX=VY=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Up) VY=-speed;
			if (is->Down) VY=speed;
		} else {
			Time--;
		}

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				CImpactAttackEnemy* enemy=(CImpactAttackEnemy*)mover;
				float dx=X-mover->X, dy=Y-mover->Y;
				if (dx!=0) {
					VX=dx/abs(dx)*speed;
					enemy->VX=-VX;
				}
				if (dy!=0) {
					VY=dy/abs(dy)*speed;
					enemy->VY=-VY;
				}
				Time=impact_time;
			}
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Y+=VY;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;

		return true;
	}
};

// 스테이지
class CImpactAttackStage : public CStage {
public:
	CImpactAttackStage() : CStage("IMPACT ATTACK") {}
	virtual void Init() {
		new CImpactAttackMan();
		for (int i=0; i<20; i++) new CImpactAttackEnemy();
	}
};


//==============================================================
// 낙하물 공격

// 낙하물
class CDroppingObject : public CMover {
public:
	float VY;
	bool Drop;

	void Init() {
		Y=2.0f;
		W=H=2;
		VY=0;
		Drop=false;
	}

	CDroppingObject(float x) {
		Texture=Game->Texture[TEX_LIGHT];
		X=x;
		Init();
	}

	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		float max_speed=0.4f;
		float max_dist=1.2f;
		if (!Drop) {
			if (is->Button[0]) Drop=true;
		} else {
			VY+=accel;
			if (VY>max_speed) VY=max_speed;
			Y+=VY;
			if (Y>MAX_Y) Init();
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					abs(X-mover->X)<max_dist &&
					abs(Y-mover->Y)<max_dist
				) {
					CAttackEnemy* enemy=(CAttackEnemy*)mover;
					enemy->Attack();
				}
			}
		}
		return true;
	}
};

// 스테이지
class CDroppingObjectStage : public CStage {
public:
	CDroppingObjectStage() : CStage("DROPPING OBJECT") {}
	virtual void Init() {
		for (int i=0; i<3; i++) new CDroppingObject(i*4+3);
		for (int i=0; i<3; i++) new CAttackEnemy();
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, 1);
			new CStaticFloor(i, MAX_Y-1);
		}
	}
};


//==============================================================
// 콤보

// 사람
class CComboCountMan : public CMover {
public:
	float VX;
	int ComboCount, ComboTime;

	CComboCountMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		ComboCount=0;
		ComboTime=0;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.6f;
		int combo_time=60;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(X-mover->X)<max_dist
			) {
				CAttackEnemy* enemy=(CAttackEnemy*)mover;
				enemy->Attack();
				ComboCount++;
				ComboTime=combo_time;
			}
		}
		
		if (ComboTime>0) ComboTime--; else ComboCount=0;

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CComboCountStage : public CStage {
public:
	CComboCountMan* Man;
	CComboCountStage() : CStage("COMBO COUNT") {}
	virtual void Init() {
		Man=new CComboCountMan();
		for (int i=0; i<3; i++) new CAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
	virtual void Draw() {
		float sw=Game->GetGraphics()->GetWidth();
		float sh=Game->GetGraphics()->GetHeight();
		char buf[100];
		sprintf_s(buf, sizeof(buf), "COMBO COUNT %d", Man->ComboCount);
		Game->Font->DrawText(buf, 0, sh*1.5f/MAX_Y, COL_BLACK);
		sprintf_s(buf, sizeof(buf), "COMBO TIME %d", Man->ComboTime);
		Game->Font->DrawText(buf, 0, sh*2.2f/MAX_Y, COL_BLACK);
	}	
};


//==============================================================
// 간격에 따른 공격 변화

// 수리검
class CAttackRangeShuriken : public CMover {
public:
	float VX;
	CAttackRangeShuriken(float x, float y, float vx) {
		Texture=Game->Texture[TEX_SHURIKEN];
		X=x;
		Y=y;
		VX=vx;
	}
	virtual bool Move(const CInputState* is) {
		float max_dist=0.6f;
		X+=VX;
		Angle+=VX*0.1f;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(X-mover->X)<max_dist
			) {
				CAttackEnemy* enemy=(CAttackEnemy*)mover;
				enemy->Attack();
				return false;
			}
		}		
		return X>-1 && X<MAX_X;
	}
};

// 사람
class CAttackRangeMan : public CMover {
public:
	float VX, DirX;
	bool Sword, PrevButton;

	CAttackRangeMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		Sword=false;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.6f;
		float near_dist=3.0f;
		float throw_speed=0.4f;

		VX=0;
		if (is->Left) {
			DirX=-1;
			VX=-speed;
		}
		if (is->Right) {
			DirX=1;
			VX=speed;
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		bool near_enemy=false;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(X-mover->X)<near_dist &&
				(X-mover->X)*DirX<=0
			) {
				near_enemy=true;
				break;
			}
		}

		if (!is->Button[0]) Sword=false;
		if (!PrevButton && is->Button[0]) {
			if (near_enemy) {
				Sword=true;
			} else {
				new CAttackRangeShuriken(X, Y, DirX*throw_speed);
			}
		}
		PrevButton=is->Button[0];

		if (Sword) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					abs(X+DirX-mover->X)<max_dist
				) {
					CAttackEnemy* enemy=(CAttackEnemy*)mover;
					enemy->Attack();
				}
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}

	virtual void Draw() {
		float x=X, angle=Angle;
		if (Sword) {
			X=x+DirX*0.75f;
			Angle=(DirX<0?-0.25f:0.25f);
			Texture=Game->Texture[TEX_SWORD];
			CMover::Draw();
		}
		Texture=Game->Texture[TEX_MAN];
		X=x;
		Angle=angle;
		CMover::Draw();
	}
};

// 스테이지
class CAttackRangeStage : public CStage {
public:
	CAttackRangeStage() : CStage("ATTACK RANGE") {}
	virtual void Init() {
		new CAttackRangeMan();
		for (int i=0; i<3; i++) new CAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 후방 공격

// 사람
class CBackAttackMan : public CMover {
public:
	float VX, DirX;
	bool Sword, PrevButton;

	CBackAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		Sword=false;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.6f;
		float throw_speed=0.4f;

		VX=0;
		if (is->Left) {
			DirX=-1;
			VX=-speed;
		}
		if (is->Right) {
			DirX=1;
			VX=speed;
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		if (!is->Button[0]) Sword=false;
		if (!PrevButton && is->Button[0]) {
			if (is->Button[1]) {
				new CAttackRangeShuriken(X, Y, -DirX*throw_speed);
			} else {
				Sword=true;
			}
		}
		PrevButton=is->Button[0]||is->Button[1];

		if (Sword) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					abs(X+DirX-mover->X)<max_dist
				) {
					CAttackEnemy* enemy=(CAttackEnemy*)mover;
					enemy->Attack();
				}
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}

	virtual void Draw() {
		float x=X, angle=Angle;
		if (Sword) {
			X=x+DirX*0.75f;
			Angle=(DirX<0?-0.25f:0.25f);
			Texture=Game->Texture[TEX_SWORD];
			CMover::Draw();
		}
		Texture=Game->Texture[TEX_MAN];
		X=x;
		Angle=angle;
		CMover::Draw();
	}
};

// 스테이지
class CBackAttackStage : public CStage {
public:
	CBackAttackStage() : CStage("BACK ATTACK") {}
	virtual void Init() {
		new CBackAttackMan();
		for (int i=0; i<3; i++) new CAttackEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 레버 흔들기 공격

// 사람
class CShakeLeverMan : public CMover {
public:
	float VX;
	bool PrevLeft, PrevRight, Attack;

	CShakeLeverMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		PrevLeft=PrevRight=Attack=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Attack=
			PrevLeft && is->Right ||
			PrevRight && is->Left;
		PrevLeft=is->Left;
		PrevRight=is->Right;
		
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 적
class CShakeLeverEnemy : public CMover {
public:
	float VX, VY;
	int State, Vitality;
	CShakeLeverMan* Man;

	void Init() {
		X=MAX_X;
		Y=MAX_Y-2;
		VX=-Game->Rand.Real1()*0.05f-0.05f;
		Angle=0;
		State=0;
		Vitality=10;
	}

	CShakeLeverEnemy(CShakeLeverMan* man) {
		Texture=Game->Texture[TEX_ENEMY1];
		Type=1;
		Man=man;
		Init();
	}

	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		float vangle=0.05f;
		float max_dist=0.4f;
		float knockout_vx=0.4f;
		float knockout_vy=-0.5f;
		switch (State) {
			case 0:
				if (abs(X-Man->X)<max_dist) {
					X=Man->X;
					if (Man->Attack) {
						Vitality--;
						if (Vitality<=0) {
							VX=knockout_vx;
							VY=knockout_vy;
							State=1;
						}
					}
				} else {
					X+=VX;
					if (X<-1) Init();
				}
				break;
			case 1:
				X+=VX;
				VY+=accel;
				Y+=VY;
				Angle+=vangle;
				if (X>MAX_X) Init();
				break;
		}
		return true;
	}
};

// 스테이지
class CShakeLeverStage : public CStage {
public:
	CShakeLeverStage() : CStage("SHAKING LEVER") {}
	virtual void Init() {
		CShakeLeverMan* man=new CShakeLeverMan();
		for (int i=0; i<3; i++) new CShakeLeverEnemy(man);
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


