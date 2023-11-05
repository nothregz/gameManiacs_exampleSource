// 아이템
class CItem : public CMover {
public:
	int Time;
	CItem(CTexture* texture) {
		Texture=texture;
		Type=2;
		Init();
	}
	void Init() {
		X=Game->Rand.Real1()*(MAX_X-1);
		Y=MAX_Y-2;
		Time=0;
	}
	virtual bool Move(const CInputState* is) {
		Angle=sin(Time*0.1f)*0.1f;
		Time++;
		return true;
	}
};

//==============================================================
// 무적 아이템

// 사람
class CInvincibleItemMan : public CMover {
public:
	float VX;
	int InvincibleTime;

	CInvincibleItemMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		InvincibleTime=0;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.6f;
		int invincible_time=180;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		if (InvincibleTime==0) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==2 &&
					abs(X-mover->X)<max_dist &&
					abs(Y-mover->Y)<max_dist
				) {
					((CItem*)mover)->Init();
					InvincibleTime=invincible_time;
				}
			}
			W=H=1;
			Y=MAX_Y-2;
			Type=0;
		} else {
			InvincibleTime--;
			W=H=2;
			Y=MAX_Y-2.5f;
			Type=1;
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CInvincibleItemStage : public CStage {
public:
	CInvincibleItemStage() : CStage("INVINCIBLE ITEM") {}
	virtual void Init() {
		new CInvincibleItemMan();
		new CItem(Game->Texture[TEX_MEDICINE]);
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 주워 던지는 아이템

// 사람
class CPickAndThrowItemMan : public CMover {
public:
	float VX, DirX;
	bool Item, PrevButton;

	CPickAndThrowItemMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		Item=false;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float throw_speed=0.2f;
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

		if (!Item) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==2 &&
					abs(X-mover->X)<max_dist &&
					abs(Y-mover->Y)<max_dist
				) {
					((CItem*)mover)->Init();
					Item=true;
				}
			}
		} else {
			if (!PrevButton && is->Button[0]) {
				new CGrenade(X, Y-1, DirX*throw_speed, -throw_speed*2);
				Item=false;
			}			
		}
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
	virtual void Draw() {
		float angle=Angle;
		if (Item) {
			Texture=Game->Texture[TEX_BOMB];
			Y=MAX_Y-3;
			Angle=0;
			CMover::Draw();
		}
		Texture=Game->Texture[TEX_MAN];
		Y=MAX_Y-2;
		Angle=angle;
		CMover::Draw();
	}
};

// 스테이지
class CPickAndThrowItemStage : public CStage {
public:
	CPickAndThrowItemStage() : CStage("THROWING") {}
	virtual void Init() {
		new CPickAndThrowItemMan();
		new CItem(Game->Texture[TEX_BOMB]);
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 떨어져 내려오는 아이템

// 떨어져 내려오는 아이템
class CFallingItem : public CMover {
public:
	float VX, DirX, VY;
	CFallingItem() {
		Texture=Game->Texture[TEX_HEART];
		Type=2;
		Init();
	}
	void Init() {
		X=Game->Rand.Real1()*(MAX_X-1);
		Y=-1;
		VX=0;
		VY=Game->Rand.Real1()*0.05f+0.05f;
		DirX=(Game->Rand.Real1()<0.5f?-1:1);
	}
	virtual bool Move(const CInputState* is) {
		float accel=0.002f;
		if (Game->Rand.Real1()<0.1f) DirX=-DirX;
		VX+=accel*DirX;
		X+=VX;
		Y+=VY;
		if (Y>=MAX_Y) Init();
		return true;
	}
};

// 사람
class CFallingItemMan : public CMover {
public:
	float VX;

	CFallingItemMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
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
				mover->Type==2 &&
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				((CFallingItem*)mover)->Init();
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CFallingItemStage : public CStage {
public:
	CFallingItemStage() : CStage("FALLING ITEM") {}
	virtual void Init() {
		new CFallingItemMan();
		for (int i=0; i<10; i++) new CFallingItem();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 특정 장소를 통과하면 등장하는 아이템

// 스위치
class CItemByPassSwitch : public CMover {
public:
	CMover* Man;
	bool PrevHit;
	CItemByPassSwitch(float x, float y, CMover* man) {
		X=x;
		Y=y;
		Man=man;
		PrevHit=false;
	}

	virtual bool Move(const CInputState* is) {
		float max_dist=0.6f;
		if (
			abs(X-Man->X)<max_dist &&
			abs(Y-Man->Y)<max_dist
		) {
			if (!PrevHit) new CItem(Game->Texture[TEX_JEWEL]);
			PrevHit=true;
		} else {
			PrevHit=false;
		}
		return true;
	}
};

// 사람
class CItemByPassMan : public CMover {
public:
	float VX;

	CItemByPassMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
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
				mover->Type==2 &&
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				i.Remove();
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CItemByPassStage : public CStage {
public:
	CItemByPassStage() : CStage("ITEM BY PASS") {}
	virtual void Init() {
		CMover* man=new CItemByPassMan();
		for (int i=0; i<MAX_X; i++) {
			CStaticFloor* floor=new CStaticFloor(i, MAX_Y-1);
			if (i==7) {
				floor->Texture=Game->Texture[TEX_ICE_FLOOR];
				floor->Color=COL_WHITE;
				new CItemByPassSwitch(i, MAX_Y-2, man);
			}
		}
	}
};


//==============================================================
// 특정 장소를 공격하면 등장하는 아이템

// 아이템
class CItemByAttackItem : public CMover {
public:
	float InitialY, VY;
	CItemByAttackItem(float x, float y) {
		Texture=Game->Texture[TEX_MUSHROOM];
		X=x;
		InitialY=Y=y;
		VY=-0.2f;
		Type=2;
	}
	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		VY+=accel;
		Y+=VY;
		if (Y>InitialY) {
			Y=InitialY;
			VY=0;
		}
		return true;
	}
};

// 블록
class CItemByAttackBlock : public CMover {
public:
	float InitialY, VY;
	CItemByAttackBlock(float x, float y) {
		Texture=Game->Texture[TEX_FLOOR];
		X=x;
		InitialY=Y=y;
		VY=0;
		Type=1;
	}
	void Attack() {
		VY=-0.1f;
		new CItemByAttackItem(X, Y-1);
	}
	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		VY+=accel;
		Y+=VY;
		if (Y>InitialY) {
			Y=InitialY;
			VY=0;
		}
		return true;
	}
};

// 사람
class CItemByAttackMan : public CMover {
public:
	float VX, VY;
	bool Jump;

	CItemByAttackMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=VY=0;
		Jump=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float jump_speed=-0.5f;
		float jump_accel=0.02f;
		float floor_dist=1.0f;
		float item_dist=0.6f;

		if (!Jump) {
			VX=0;
			if (is->Left) VX=-speed;
			if (is->Right) VX=speed;
			if (is->Button[0]) VY=jump_speed;
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(X-mover->X)<floor_dist &&
				abs(Y-mover->Y)<floor_dist
			) {
				if (VX<0) {
					X=mover->X+floor_dist;
				} else {
					X=mover->X-floor_dist;
				}
			}
		}

		VY+=jump_accel;
		Y+=VY;
		
		Jump=true;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 && 
				abs(X-mover->X)<floor_dist &&
				abs(Y-mover->Y)<floor_dist
			) {
				if (VY<0) {
					((CItemByAttackBlock*)mover)->Attack();
					Y=mover->Y+floor_dist;
				} else {
					Jump=false;
					Y=mover->Y-floor_dist;
				}
				VY=0;
			}
		}

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==2 &&
				abs(X-mover->X)<item_dist &&
				abs(Y-mover->Y)<item_dist
			) {
				i.Remove();
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CItemByAttackStage : public CStage {
public:
	CItemByAttackStage() : CStage("ITEM BY ATTACK") {}
	virtual void Init() {
		new CItemByAttackMan();
		for (int i=0; i<3; i++) new CItemByAttackBlock(i*5+2.5f, MAX_Y-6);
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1, 1);
	}
};

