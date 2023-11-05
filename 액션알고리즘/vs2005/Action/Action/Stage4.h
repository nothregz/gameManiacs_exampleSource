//==============================================================
// 벽에 매달리기

// 사람
class CHangOnWallMan : public CMover {
public:
	float VX, VY;
	bool PrevButton, Jump;

	CHangOnWallMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		PrevButton=false;
		Jump=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float jump_speed=-0.3f;
		float jump_accel=0.01f;
		float max_diff=0.6f;
		float min_dist=0.0f;
		float max_dist=1.3f;
		float adjust_dist=1.0f;
		bool hit_up=false;
		bool hit_left=false;
		bool hit_right=false;
		bool hit_down=false;
		
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover->Type==1) {
				if (
					abs(X-mover->X)<max_diff &&
					Y-mover->Y>=min_dist &&
					Y-mover->Y<max_dist
				) {
					hit_up=true;
					if (VY<0) {
						Y=mover->Y+adjust_dist;
						Jump=false;
					}
				}
				if (
					abs(X-mover->X)<max_diff &&
					mover->Y-Y>=min_dist &&
					mover->Y-Y<max_dist
				) {
					hit_down=true;
					if (VY>0) {
						Y=mover->Y-adjust_dist;
						Jump=false;
					}
				}
				if (
					abs(Y-mover->Y)<max_diff &&
					X-mover->X>=min_dist &&
					X-mover->X<max_dist
				) {
					hit_left=true;
					if (VX<0) {
						X=mover->X+adjust_dist;
						Jump=false;
					}
				}
				if (
					abs(Y-mover->Y)<max_diff &&
					mover->X-X>=min_dist &&
					mover->X-X<max_dist
				) {
					hit_right=true;
					if (VX>0) {
						X=mover->X-adjust_dist;
						Jump=false;
					}
				}
			}
		}

		if (Jump) {
			VY+=jump_accel;
			if (VY>-jump_speed) VY=-jump_speed;
		} else {
			VX=VY=0;
			if (hit_up || hit_down) {
				if (is->Left && !hit_left) VX=-speed;
				if (is->Right && !hit_right) VX=speed;
				if (
					!PrevButton && is->Button[0] && 
					!hit_left && !hit_right
				) {
					VY=hit_up?0:jump_speed;
					Jump=true;
				}
			}
			if (hit_left || hit_right) {
				if (is->Up && !hit_up) VY=-speed;
				if (is->Down && !hit_down) VY=speed;
				if (
					!PrevButton && is->Button[0] && 
					!hit_up
				) {
					VX=hit_left?speed:-speed;
					VY=jump_speed;
					Jump=true;
				}
			}
			if (!hit_up && !hit_down && !hit_left && !hit_right) {
				Jump=true;
			}
		}
		

		X+=VX;
		Y+=VY;
		PrevButton=is->Button[0];

		if (Jump || hit_up || hit_down) {
			Angle=VX/speed*0.1f;
		} else {
			Angle=hit_left?-0.1f:0.1f;
		}
		return true;
	}
};

// 스테이지
class CHangOnWallStage : public CStage {
	CHangOnWallMan* Man;
public:
	CHangOnWallStage() : CStage("HANGING ON WALL") {}
	virtual void Init() {
		Man=new CHangOnWallMan();
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, 1, 1);
			new CStaticFloor(i, MAX_Y-1, 1);
		}
		for (int i=1; i<MAX_Y; i++) {
			new CStaticFloor(0, i, 1);
			new CStaticFloor(MAX_X-1, i, 1);
		}
		for (int i=4; i<MAX_Y-3; i++) {
			for (int j=0; j<3; j++) {
				new CStaticFloor(j*4+3, i, 1);
			}
		}
	}
};


//==============================================================
// 바위 떨어뜨리기

// 사람
class CDropRockMan : public CMover {
public:
	float VX, VY;

	CDropRockMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=(MAX_Y-1)/2;
		VX=0;
		VY=0;
		DrawOrder=1;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=1.0f;

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

		if (VX!=0) Y=(int)(Y+0.5f);
		if (VY!=0) X=(int)(X+0.5f);
		
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==2 &&
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				X-=VX;
				Y-=VY;
				break;
			}
		}
		
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 바위
class CDropRock : public CMover {
	int State, Time;
public:
	CDropRock(float x, float y, int type) {
		Texture=Game->Texture[TEX_ROCK];
		X=x;
		Y=y;
		Type=type;
		DrawOrder=1;
		State=0;
	}
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=1.2f;

		bool drop=true;
		if (Y>=MAX_Y-1) {
			drop=false;
		} else {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					X==mover->X &&
					mover->Y-Y>0 &&
					mover->Y-Y<=max_dist
				) {
					drop=false;
					break;
				}
			}
		}
		switch (State) {
			case 0:
				if (drop) {
					Time=60;
					State=1;
				}
				break;
			case 1:
				Time--;
				if (Time==0) State=2;
				break;
			case 2:
				if (!drop) {
					State=0;
				} else {
					Y+=speed;
				}
				break;
		}

		if (State==1) Angle=sin(Time*0.5f)*0.05f; else Angle=0;
		return true;
	}
};

// 흙
class CDropRockClay : public CMover {
	CDropRockMan* Man;
public:
	CDropRockClay(float x, float y, int type, CDropRockMan* man) {
		Texture=Game->Texture[TEX_CLAY];
		X=x;
		Y=y;
		Type=type;
		Man=man;
		Color=COL_MGRAY;
	}
	virtual bool Move(const CInputState* is) {
		float max_dist=0.5f;
		return 
			abs(X-Man->X)>=max_dist ||
			abs(Y-Man->Y)>=max_dist;
	}
};

// 스테이지
class CDropRockStage : public CStage {
	CDropRockMan* Man;
public:
	CDropRockStage() : CStage("DROPPING ROCK") {}
	virtual void Init() {
		Man=new CDropRockMan();
		for (int j=1; j<MAX_Y; j++) {
			for (int i=0; i<MAX_X; i++) {
				if (i%7==0 && j%3==0) new CDropRock(i, j, 2); 
				else new CDropRockClay(i, j, 1, Man);
			}
		}
	}
};


//==============================================================
// 물체 밀기

// 사람
class CPushObjectMan : public CMover {
public:
	float VX, VY;

	CPushObjectMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=(MAX_Y-1)/2;
		VX=0;
		VY=0;
		DrawOrder=1;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.9f;

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

		int count=0;
		CMover* object=NULL;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 && 
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				object=mover;
				count++;
				if (count>1) {
					X-=VX;
					Y-=VY;
					break;
				}
			}
		}
		
		if (count==1) {
			bool movable=true;
			float x=object->X+VX;
			float y=object->Y+VY;
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					x<0 || x>MAX_X-1 || 
					y<0 || y>MAX_Y-1 ||
					mover!=object &&
					mover->Type==1 &&
					abs(x-mover->X)<max_dist &&
					abs(y-mover->Y)<max_dist
				) {
					movable=false;
					break;
				}				
			}
			if (movable) {
				object->X=x;
				object->Y=y;
			} else {
				X-=VX;
				Y-=VY;
			}
		}
				
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 물체
class CPushObject : public CMover {
public:
	CPushObject(float x, float y, int type) {
		Texture=Game->Texture[TEX_OBJECT];
		X=x;
		Y=y;
		Type=type;
	}
};

// 스테이지
class CPushObjectStage : public CStage {
	CPushObjectMan* Man;
public:
	CPushObjectStage() : CStage("PUSHING OBJECT") {}
	virtual void Init() {
		Man=new CPushObjectMan();
		for (int j=1; j<MAX_Y; j++) {
			for (int i=0; i<MAX_X; i++) {
				if (
					(i!=(MAX_X-1)/2 || j!=(MAX_Y-1)/2) &&
					Game->Rand.Real1()<0.1f
				) {
					new CPushObject(i, j, 1);
				}
			}
		}
	}
};


//==============================================================
// 얼음 밀기

// 얼음
class CPushIce : public CMover {
public:
	float VX, VY;
	CPushIce(float x, float y, int type) {
		Texture=Game->Texture[TEX_ICE];
		X=x;
		Y=y;
		VX=VY=0;
		Type=type;
	}
	virtual bool Move(const CInputState* is) {
		float max_dist=0.9f;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Y+=VY;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover!=this &&
				mover->Type==1 && 
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				X-=VX;
				Y-=VY;
				VX=VY=0;
				X=(int)(X+0.5f);
				Y=(int)(Y+0.5f);
				break;
			}
		}
		return true;
	}
};

// 사람
class CPushIceMan : public CMover {
public:
	float VX, VY, DirX, DirY;
	bool PrevButton;

	CPushIceMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=(MAX_Y-1)/2;
		VX=0;
		VY=0;
		DirX=-1;
		DirY=0;
		DrawOrder=1;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float ice_speed=0.4f;
		float max_dist=0.9f;

		VX=VY=0;
		if (is->Left) {
			DirX=-1;
			DirY=0;
			VX=-speed; 
		} else
		if (is->Right) {
			DirX=1;
			DirY=0;
			VX=speed; 
		} else
		if (is->Up) {
			DirX=0;
			DirY=-1;
			VY=-speed;
		} else
		if (is->Down) {
			DirX=0;
			DirY=1;
			VY=speed;
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Y+=VY;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 && 
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				X-=VX;
				Y-=VY;
				break;
			}
		}

		if (!PrevButton && is->Button[0]) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 && 
					abs(X+DirX-mover->X)<max_dist &&
					abs(Y+DirY-mover->Y)<max_dist
				) {
					CPushIce* ice=(CPushIce*)mover;
					ice->VX=DirX*ice_speed;
					ice->VY=DirY*ice_speed;
					break;
				}
			}
		}

		PrevButton=is->Button[0];
				
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CPushIceStage : public CStage {
	CPushIceMan* Man;
public:
	CPushIceStage() : CStage("PUSHING ICE") {}
	virtual void Init() {
		Man=new CPushIceMan();
		for (int j=1; j<MAX_Y; j++) {
			for (int i=0; i<MAX_X; i++) {
				if (
					(i!=(MAX_X-1)/2 || j!=(MAX_Y-1)/2) &&
					Game->Rand.Real1()<0.1f
				) {
					new CPushIce(i, j, 1);
				}
			}
		}
	}
};


//==============================================================
// 자동 구덩이

class CAutoHoleEnemy;

// 자동 구덩이
class CAutoHole : public CMover {
public:
	int State, Time;
	float Depth;
	CAutoHoleEnemy* Enemy;
	
	CAutoHole(float x, float y, int type) {
		Texture=Game->Texture[TEX_FLOOR];
		X=x;
		Y=y;
		Type=type;
		State=0;
		Depth=0;
		Enemy=NULL;
	}
	void Dig() {
		int dig_time=20;
		if (State==0 && !Enemy) {
			Time=dig_time;
			State=1;
		}
	}
	virtual bool Move(const CInputState* is) {
		int dig_time=20;
		int wait_time=120;
		int fill_time=20;
		switch (State) {
			case 1:
				Time--;
				Depth=(float)(dig_time-Time)/dig_time;
				if (Time==0) {
					Depth=1;
					Time=wait_time;
					State=2;
				}
				break;
			case 2:
				Time--;
				if (Time==0) {
					Time=fill_time;
					State=3;
				}
				break;
			case 3:
				Time--;
				Depth=(float)Time/fill_time;
				if (Time==0) {
					Enemy=NULL;
					Depth=0;
					State=0;
				}
				break;
		}
		return true;
	}
	virtual void Draw() {
		Texture=Game->Texture[TEX_FLOOR];
		Color=COL_BLACK;
		H=1;
		CMover::Draw();
		Texture=Game->Texture[TEX_FILL];
		Color=COL_WHITE;
		float y=Y;
		Y+=(Depth-1)*0.5f;
		H=Depth;
		CMover::Draw();
		Y=y;
	}
};

// 적
class CAutoHoleEnemy : public CMover {
public:
	float VX;
	CAutoHole* Hole;
	int State, Time;

	void Init() {
		X=MAX_X;
		Y=MAX_Y-2;
		VX=-Game->Rand.Real1()*0.05f-0.05f;
		State=0;
	}

	CAutoHoleEnemy() {
		Texture=Game->Texture[TEX_ENEMY1];
		Init();
	}

	virtual bool Move(const CInputState* is) {
		float max_dist=0.2f;
		float fall_speed=0.1f;
		int fall_time=60;

		switch (State) {
			case 0:
				X+=VX;
				if (X<-1) X=MAX_X;
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==1 &&
						abs(X-mover->X)<max_dist
					) {
						Hole=(CAutoHole*)mover;
						if (Hole->State==2 && !Hole->Enemy) {
							X=Hole->X;
							Hole->Enemy=this;
							State=1;
							break;
						}
					}
				}
				break;
			case 1:
				Y+=fall_speed;
				if (Y>=Hole->Y) {
					Time=fall_time;
					State=2;
				}
				break;
			case 2:
				Time--;
				if (Time==0) State=3;
				break;
			case 3:
				Y-=fall_speed;
				if (Y<=Hole->Y-1) {
					X=Hole->X-max_dist;
					Y=Hole->Y-1;
					Hole->Enemy=NULL;
					State=0;
				}
				break;
		}

		if (State!=0 && Hole->Depth==0) Init();

		return true;
	}
};

// 사람
class CAutoHoleMan : public CMover {
public:
	float VX;
	bool PrevButton;

	CAutoHoleMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		PrevButton=false;
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
				CAutoHole* hole=(CAutoHole*)mover;
				if (hole->State!=0 && !hole->Enemy) {
					X-=VX;
					break;
				}
			}
		}

		if (!PrevButton && (is->Button[0] || is->Button[1])) {
			X=(int)(X+0.5f);
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					(is->Button[0] && abs(X-1-mover->X)<max_dist) ||
					(is->Button[1] && abs(X+1-mover->X)<max_dist)
				) {
					CAutoHole* hole=(CAutoHole*)mover;
					hole->Dig();
					break;
				}
			}
		}
		
		PrevButton=is->Button[0]||is->Button[1];
				
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CAutoHoleStage : public CStage {
	CAutoHoleMan* Man;
public:
	CAutoHoleStage() : CStage("AUTOMATIC HOLE") {}
	virtual void Init() {
		Man=new CAutoHoleMan();
		for (int i=0; i<MAX_X; i++) new CAutoHole(i, MAX_Y-1, 1);
		for (int i=0; i<3; i++) new CAutoHoleEnemy();
	}
};


//==============================================================
// 수동 구덩이

class CManualHoleEnemy;

// 수동 구덩이
class CManualHole : public CMover {
public:
	float Size;
	CManualHoleEnemy* Enemy;

	CManualHole(float x, float y, int type) {
		Texture=Game->Texture[TEX_HOLE];
		X=x;
		Y=y;
		Type=type;
		Size=0.2f;
		W=H=Size;
		Enemy=NULL;
	}
	void Dig() {
		float dig_speed=0.2f;
		Size+=dig_speed;
		if (Size>1) Size=1;
		W=H=Size;
	}
	void Fill() {
		float fill_speed=0.2f;
		Size-=fill_speed;
		if (Size<0.1f) Size=0;
		W=H=Size;
	}
	bool Move(const CInputState* is) {
		return Size>0 || Enemy;
	}
};

// 적
class CManualHoleEnemy : public CMover {
	float VX;
	int State, Time;
	CManualHole* Hole;
public:
	void Init() {
		VX=Game->Rand.Real1()*0.05f+0.05f;
		if (Game->Rand.Real1()<0.5f) {
			X=-1;
		} else {
			X=MAX_X;
			VX=-VX;
		}
		Y=Game->Rand.Real1()*(MAX_Y-1);
		W=H=1;
		Angle=0;
		State=0;
	}

	CManualHoleEnemy() {
		Texture=Game->Texture[TEX_ENEMY0];
		Init();
	}

	bool Move(const CInputState* is) {
		float max_dist=0.4f;
		float fall_vangle=0.05f;
		int fall_time=120;

		switch (State) {
			case 0:
				W=H=1;
				X+=VX;
				if (X<-1 || X>MAX_X) Init();
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==1 &&
						abs(X-mover->X)<max_dist &&
						abs(Y-mover->Y)<max_dist
					) {
						Hole=(CManualHole*)mover;
						if (Hole->Size==1 && !Hole->Enemy) {
							X=Hole->X;
							Y=Hole->Y;
							Hole->Enemy=this;
							State=1;
							break;
						}
					}
				}
				break;
			case 1:
				W=H=Hole->Size;
				Angle+=fall_vangle;
				if (Angle>=0.5f) {
					Time=fall_time;
					State=2;
				}
				break;
			case 2:
				W=H=Hole->Size;
				Time--;
				if (Time==0) State=3;
				break;
			case 3:
				W=H=Hole->Size;
				Angle-=fall_vangle;
				if (Angle<=0) {
					Angle=0;
					X=Hole->X+max_dist*VX/abs(VX);
					Hole->Enemy=NULL;
					State=0;
				}
				break;
		}

		if (State!=0 && Hole->Size==0) Init();
		
		return true;		
	}
};

// 사람
class CManualHoleMan : public CMover {
public:
	float VX, VY, DirX, DirY;
	bool PrevButton;

	CManualHoleMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=(MAX_Y-1)/2;
		VX=VY=0;
		DirX=-1;
		DirY=0;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=1.0f;

		VX=VY=0;
		if (is->Left) {
			DirX=-1;
			DirY=0;
			VX=-speed; 
		} else
		if (is->Right) {
			DirX=1;
			DirY=0;
			VX=speed; 
		} else
		if (is->Up) {
			DirX=0;
			DirY=-1;
			VY=-speed;
		} else
		if (is->Down) {
			DirX=0;
			DirY=1;
			VY=speed;
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Y+=VY;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				X-=VX;
				Y-=VY;
				break;
			}
		}

		CManualHole* hole=NULL;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(X+DirX-mover->X)<max_dist &&
				abs(Y+DirY-mover->Y)<max_dist
			) {
				hole=(CManualHole*)mover;
				break;
			}
		}
		
		if (!PrevButton && is->Button[0]) {
			if (hole) {
				hole->Dig();
			} else {
				new CManualHole(X+DirX, Y+DirY, 1);
			}
		}
		if (!PrevButton && is->Button[1]) {
			if (hole) hole->Fill();
		}

		PrevButton=is->Button[0]||is->Button[1];
				
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CManualHoleStage : public CStage {
	CManualHoleMan* Man;
public:
	CManualHoleStage() : CStage("MANUAL HOLE") {}
	virtual void Init() {
		Man=new CManualHoleMan();
		for (int i=0; i<10; i++) new CManualHoleEnemy();
	}
};


//==============================================================
// 로프 치기

// 로프
class CPlaceRope : public CMover {
public:
	float RootX, RootY, VX, VY;
	int State;
	CPlaceRope(float root_x, float root_y, float angle) {
		float speed=0.4f;
		X=RootX=root_x;
		Y=RootY=root_y;
		Angle=angle;
		float rad=angle*D3DX_PI*2;
		VX=speed*sinf(rad);
		VY=-speed*cosf(rad);
		Type=2;
		State=0;
	}
	virtual bool Move(const CInputState* is) {
		float max_dist=0.5f;
		switch (State) {
			case 0:
				X+=VX;
				Y+=VY;
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==1 &&
						abs(mover->X-X)<max_dist &&
						abs(mover->Y-Y)<max_dist
					) {
						State=1;
					}
				}
				break;
		}
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
			(RootX-s+0.5f)*w, (X-s+0.5f)*w, 
			(RootX+s+0.5f)*w, (X+s+0.5f)*w
		};
		float y[]={
			(RootY-c+0.5f)*h, (Y-c+0.5f)*h, 
			(RootY+c+0.5f)*h, (Y+c+0.5f)*h
		};
		Game->Texture[TEX_FILL]->Draw(
			x[0], y[0], Color, 0, 0, 
			x[1], y[1], Color, 0, 0, 
			x[2], y[2], Color, 0, 0, 
			x[3], y[3], Color, 0, 0
		);
	}
};

// 사람
class CPlaceRopeMan : public CMover {
public:
	float VX, VY, DirX;
	bool PrevButton;

	CPlaceRopeMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=VY=0;
		DirX=-1;
		PrevButton=false;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float jump_speed=-0.4f;
		float jump_accel=0.02f;
		float rope_angle=0.15f;
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

		X+=VX;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(mover->X-X)<max_dist &&
				abs(mover->Y-Y)<max_dist
			) {
				X=mover->X+(X<mover->X?-max_dist:max_dist);
			}
		}

		VY+=jump_accel;
		if (VY>-jump_speed) VY=-jump_speed;
		Y+=VY;
		
		if (!is->Down) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (mover->Type==2) {
					CPlaceRope* rope=(CPlaceRope*)mover;
					float rope_y=
						rope->RootY+
						(rope->RootX-X)/tanf(rope->Angle*D3DX_PI*2);
					if (
						(rope->RootX<=X && X<=rope->X ||
						rope->X<=X && X<=rope->RootX) &&
						abs(rope_y-Y)<max_dist
					) {
						Y=rope_y;
						break;
					}
				}
			}
		}

		if (!PrevButton && is->Button[0]) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (mover->Type==2) i.Remove();
			}
			new CPlaceRope(X, Y, DirX*rope_angle);
		}
		PrevButton=is->Button[0];

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 &&
				abs(mover->X-X)<max_dist &&
				abs(mover->Y-Y)<max_dist
			) {
				Y=mover->Y+(Y<mover->Y?-max_dist:max_dist);
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CPlaceRopeStage : public CStage {
public:
	CPlaceRopeStage() : CStage("SETTING ROPE") {}
	virtual void Init() {
		new CPlaceRopeMan();
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, 1, 1);
			new CStaticFloor(i, MAX_Y-1, 1);
		}
		for (int i=1; i<MAX_Y; i++) {
			new CStaticFloor(0, i, 1);
			new CStaticFloor(MAX_X-1, i, 1);
			if (i%4==0) {
				for (int j=1; j<7-i/2; j++) {
					new CStaticFloor(j, i, 1);
					new CStaticFloor(MAX_X-1-j, i, 1);					
				}
			}
		}
	}
};


//==============================================================
// 발판 만들기

// 발판
class CFoothold : public CMover {
	int Time;
public:
	CFoothold(float x, float y, int type) {
		Texture=Game->Texture[TEX_FOOTHOLD];
		X=x;
		Y=y;
		W=H=3;
		Type=type;
		Time=180;
		Color=COL_MGRAY;
	}
	virtual bool Move(const CInputState* is) {
		Time--;
		return Time>0;
	}
};

// 사람
class CMakeFootholdMan : public CMover {
public:
	float VX, VY, DirX;
	bool PrevButton;

	CMakeFootholdMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=MAX_Y-2;
		VX=0;
		VY=0;
		DirX=-1;
		PrevButton=false;
		DrawOrder=1;
	}
	
	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float jump_speed=-0.4f;
		float jump_accel=0.02f;
		float floor_max_x=0.6f;
		float floor_min_y=0.5f;
		float floor_max_y=1.0f;
		float foothold_max_x=2.0f;
		float foothold_min_y=0.0f;
		float foothold_r=2.0f;

		VX=0;
		if (is->Left) {
			DirX=-1;
			VX=-speed;
		}
		if (is->Right) {
			DirX=1;
			VX=speed;
		}
		if (!PrevButton && is->Button[0]) {
			new CFoothold(X+DirX*2, Y+0.5f, 2);
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		VY+=jump_accel;
		if (VY>-jump_speed) VY=-jump_speed;
		
		Y+=VY;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;
		
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover->Type==1) {
				if (
					abs(mover->X-X)<floor_max_x &&
					mover->Y-Y>floor_min_y &&
					mover->Y-Y<floor_max_y
				) {
					Y=mover->Y-floor_max_y;
				}
			}
			if (mover->Type==2) {
				float dx=abs(mover->X-X);
				if (dx<foothold_r) {
					float foothold_max_y=
						foothold_min_y+sqrt(foothold_r*foothold_r-dx*dx);
					if (
						mover->Y-Y>foothold_min_y &&
						mover->Y-Y<foothold_max_y
					) {	
						Y=mover->Y-foothold_max_y;
					}
				}
			}
		}

		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CMakeFootholdStage : public CStage {
	CMakeFootholdMan* Man;
public:
	CMakeFootholdStage() : CStage("FOOTHOLD") {}
	virtual void Init() {
		Man=new CMakeFootholdMan();
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, MAX_Y-1, 1);
		}
	}
};


//==============================================================
// 벽 쓰러뜨리기

// 벽(패널)
class CFlappingPanel : public CMover {
public:
	float VAngle;
	int State, Time;
	
	CFlappingPanel(float x, float y, int type) {
		Texture=Game->Texture[TEX_PANEL];
		Color=COL_MGRAY;
		X=x;
		Y=y;
		W=H=3;
		Type=type;
		State=0;
	}
	void GetUp() {
		float vangle=0.125f;
		if (State==2) {
			VAngle=(Angle<0)?vangle:-vangle;
			State=3;
		}
	}
	void GetDown(float vx) {
		float vangle=0.05f;
		if (State==0 && vx!=0) {
			VAngle=(vx<0)?-vangle:vangle;
			State=1;
		}
	}
	virtual bool Move(const CInputState* is) {
		switch (State) {
			case 1:
				Angle+=VAngle;
				if (abs(Angle)>=0.25f) State=2;
				break;
			case 3:
				Angle+=VAngle;
				if (abs(Angle)<0.1f) {
					Angle=0;
					State=0;
				}
				break;
		}
		return true;
	}
};

// 적
class CFlappingPanelEnemy : public CMover {
public:
	float VX, VY;
	int State, Time;

	void Init() {
		X=MAX_X;
		Y=MAX_Y-2;
		VX=-Game->Rand.Real1()*0.05f-0.05f;
		W=H=1;
		Angle=0;
		State=0;
		DrawOrder=1;
	}

	CFlappingPanelEnemy() {
		Texture=Game->Texture[TEX_ENEMY1];
		Init();
	}

	virtual bool Move(const CInputState* is) {
		float max_dist=1.6f;
		float shrink=0.2f;
		float jump_vx=-0.3f;
		float jump_vy=-0.6f;
		float jump_accel=0.02f;

		switch (State) {
			case 0:
				X+=VX;
				if (X<-1) X=MAX_X;
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==1 &&
						abs(mover->X-X)<max_dist
					) {
						CFlappingPanel* panel=(CFlappingPanel*)mover;
						if (
							panel->Angle>0 && panel->X<X ||
							panel->Angle<0 && X<panel->X
						) {
							if (panel->State==1) {
								State=1;
								break;
							}
							if (panel->State==3) {
								VX=jump_vx*panel->Angle/abs(panel->Angle);
								VY=jump_vy;
								Angle=0.5f;
								State=2;
								break;
							}
							break;
						}
					}
				}
				break;
			case 1:
				H-=shrink;
				Y+=shrink*0.5f;
				if (H<=0) Init();
				break;	
			case 2:
				X+=VX;
				VY+=jump_accel;
				Y+=VY;
				if (X<-1 || X>MAX_X) Init();
				break;
		}

		return true;
	}
};

// 사람
class CFlappingPanelMan : public CMover {
public:
	float VX, VY;
	bool PrevButton;

	CFlappingPanelMan() {
		Texture=Game->Texture[TEX_MAN];
		X=1;
		Y=MAX_Y-2;
		VX=VY=0;
		PrevButton=false;
		DrawOrder=1;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.7f;

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
				CFlappingPanel* panel=(CFlappingPanel*)mover;
				panel->GetDown(VX);
			}
		}
		
		if (!PrevButton && is->Button[0]) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (mover->Type==1) {
					CFlappingPanel* panel=(CFlappingPanel*)mover;
					panel->GetUp();
				}
			}
		}

		PrevButton=is->Button[0];
				
		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CFlappingPanelStage : public CStage {
	CFlappingPanelMan* Man;
public:
	CFlappingPanelStage() : CStage("FLAPPING PANEL") {}
	virtual void Init() {
		Man=new CFlappingPanelMan();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
		for (int i=0; i<3; i++) new CFlappingPanel(i*4+3, MAX_Y-1.5f, 1);
		for (int i=0; i<3; i++) new CFlappingPanelEnemy();
	}
};


//==============================================================
// 지면 떨어뜨리기

// 지면
class CDropLand : public CMover {
public:
	int XSpan, YSpan, State, Mark;
	CDropLand(float x, float y, int xs, int ys) {
		Texture=Game->Texture[TEX_CLAY];
		Color=COL_DGRAY;
		X=x;
		Y=y;
		XSpan=xs;
		YSpan=ys;
		W=H=0;
		State=0;
	}
	void Drop() {
		if (State==0) State=1;
	}
	virtual bool Move(const CInputState* is) {
		float drop_speed=0.05f;
		if (State==1) {
			W+=drop_speed;
			H+=drop_speed;
			if (W>=1) {
				W=H=1;
				State=2;
			}
		}
		return true;
	}
	virtual void Draw() {
		float x=X, y=Y;
		for (int j=0; j<YSpan; j++) {
			for (int i=0; i<XSpan; i++) {
				X=x+i;
				Y=y+j;
				CMover::Draw();
			}
		}
		X=x;
		Y=y;
	}
};

// 금
class CDropLandCrack : public CMover {
public:
	CDropLand* Land[2];
	int XSpan, YSpan, State;
	CDropLandCrack(float x, float y, int xs, int ys, float angle) {
		Texture=Game->Texture[TEX_CRACK];
		Color=COL_DGRAY;
		X=x;
		Y=y;
		XSpan=xs;
		YSpan=ys;
		Angle=angle;
		DrawOrder=1;
		W=H=0;
		State=0;
	}
	void Crack() {
		if (State==0) State=1;
	}
	virtual bool Move(const CInputState* is) {
		float crack_speed=0.05f;
		if (State==1) {
			W+=crack_speed;
			H+=crack_speed;
			if (W>=1) {
				W=H=1;
				State=2;
			}
		}
		return true;
	}
	virtual void Draw() {
		float x=X, y=Y;
		for (int j=0; j<YSpan; j++) {
			for (int i=0; i<XSpan; i++) {
				X=x+i;
				Y=y+j;
				CMover::Draw();
			}
		}
		X=x;
		Y=y;
	}
};

// 말뚝
class CDropLandPile : public CMover {
public:
	CDropLandCrack* Crack[4];
	CDropLandPile(float x, float y, int type) {
		Texture=Game->Texture[TEX_PILE];
		X=x;
		Y=y;
		Type=type;
		DrawOrder=1;
	}
};

// 스테이지
class CDropLandStage : public CStage {
	CDropLand* Land[12];
	CDropLandCrack* Crack[17];
public:
	CDropLandStage() : CStage("DROPPING LAND") {}
	virtual void Init();
	void MarkLand(CDropLand* land, int mark) {
		int crack_count=17;
		if (land->State==0 && land->Mark<0) {
			land->Mark=mark;
			for (int i=0; i<crack_count; i++) {
				if (Crack[i]->State==0) {
					if (Crack[i]->Land[0]==land) {
						MarkLand(Crack[i]->Land[1], mark);
					}
					if (Crack[i]->Land[1]==land) {
						MarkLand(Crack[i]->Land[0], mark);
					}
				}
			}
		}
	}
	void DropLand() {
		int land_count=12;		
		for (int i=0; i<land_count; i++) Land[i]->Mark=-1;

		int mark=0;
		for (int i=0; i<land_count; i++) {
			if (Land[i]->State==0 && Land[i]->Mark<0) {
				MarkLand(Land[i], mark);
				mark++;
			}
		}

		if (mark>=2) {
			int min_mark=0;
			int min_count=land_count;
			for (int j=0; j<mark; j++) {
				int count=0;
				for (int i=0; i<land_count; i++) {
					if (Land[i]->Mark==j) count++;
				}
				if (count<min_count) {
					min_count=count;
					min_mark=j;
				}
			}

			for (int i=0; i<land_count; i++) {
				if (Land[i]->Mark==min_mark) Land[i]->Drop();
			}
		}		
	}
};

// 사람
class CDropLandMan : public CMover {
public:
	float VX, VY, DirX, DirY;
	bool PrevButton;
	CDropLandStage* Stage;

	CDropLandMan(CDropLandStage* stage) {
		Stage=stage;
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=(MAX_Y-1)/2;
		VX=0;
		VY=0;
		DirX=-1;
		DirY=0;
		DrawOrder=2;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.9f;

		VX=VY=0;
		if (is->Left) {
			DirX=-1;
			DirY=0;
			VX=-speed; 
		} else
		if (is->Right) {
			DirX=1;
			DirY=0;
			VX=speed; 
		} else
		if (is->Up) {
			DirX=0;
			DirY=-1;
			VY=-speed;
		} else
		if (is->Down) {
			DirX=0;
			DirY=1;
			VY=speed;
		}

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		Y+=VY;
		if (Y<0) Y=0;
		if (Y>MAX_Y-1) Y=MAX_Y-1;

		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==1 && 
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				X-=VX;
				Y-=VY;
				break;
			}
		}

		if (!PrevButton && is->Button[0]) {
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 && 
					abs(X+DirX-mover->X)<max_dist &&
					abs(Y+DirY-mover->Y)<max_dist
				) {
					CDropLandPile* pile=(CDropLandPile*)mover;
					if (DirX<0) pile->Crack[0]->Crack(); else
					if (DirX>0) pile->Crack[1]->Crack(); else
					if (DirY<0) pile->Crack[2]->Crack(); else
					if (DirY>0) pile->Crack[3]->Crack();
					Stage->DropLand();
					break;
				}
			}
		}

		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
void CDropLandStage::Init() {
	new CDropLandMan(this);

	for (int n=0, j=0; j<3; j++) {
		for (int i=0; i<4; i++, n++) {
			Land[n]=new CDropLand(i*4, j*4, 4, 4);
		}
	}

	for (int n=0, j=0; j<2; j++) {
		for (int i=0; i<4; i++, n++) {
			Crack[n]=new CDropLandCrack(i*4+0.5f, j*4+3.5f, 3, 1, 0.25f);
			Crack[n]->Land[0]=Land[i+j*4];
			Crack[n]->Land[1]=Land[i+j*4+4];
		}
	}

	for (int n=0, j=0; j<3; j++) {
		for (int i=0; i<3; i++, n++) {
			Crack[n+8]=new CDropLandCrack(i*4+3.5f, j*4+0.5f, 1, 3, 0);
			Crack[n+8]->Land[0]=Land[i+j*4];
			Crack[n+8]->Land[1]=Land[i+j*4+1];
		}
	}

	for (int n=0, j=0; j<2; j++) {
		for (int i=0; i<3; i++, n++) {
			CDropLandPile* pile=new CDropLandPile(i*4+3.5f, j*4+3.5f, 1);
			pile->Crack[0]=Crack[i+j*4];
			pile->Crack[1]=Crack[i+j*4+1];
			pile->Crack[2]=Crack[8+i+j*3];
			pile->Crack[3]=Crack[8+i+j*3+3];
		}
	}
}

