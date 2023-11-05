// 적
class CWeaponEnemy : public CMover {
public:
	float VX, VY;
	int State;
	bool RemoveWeapon;

	void Init() {
		X=MAX_X;
		Y=MAX_Y-2;
		VX=-Game->Rand.Real1()*0.05f-0.05f;
		Angle=0;
		State=0;
	}

	CWeaponEnemy(bool remove_weapon=false) {
		Texture=Game->Texture[TEX_ENEMY1];
		RemoveWeapon=remove_weapon;
		Init();
	}

	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		float vangle=0.05f;
		float max_dist=0.5f;
		float knockout_vx=0.4f;
		float knockout_vy=-0.5f;
		switch (State) {
			case 0:
				X+=VX;
				if (X<-1) Init();
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==1 &&
						abs(X-mover->X)<max_dist*mover->W &&
						abs(Y-mover->Y)<max_dist*mover->H
					) {
						VX=knockout_vx;
						VY=knockout_vy;
						State=1;
						if (RemoveWeapon) i.Remove();
						break;
					}
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


//==============================================================
// 검

// 검
class CSwordMan;
class CSword : public CMover {
public:
	CSwordMan* Man;
	CSword(CSwordMan* man);
	virtual bool Move(const CInputState* is);
};

// 사람
class CSwordMan : public CMover {
public:
	float VX, DirX;
	bool Sword;

	CSwordMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		Sword=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;

		if (!Sword) {
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

			if (is->Button[0]) {
				new CSword(this);
				Sword=true;
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 검
CSword::CSword(CSwordMan* man) {
	Man=man;
	Texture=Game->Texture[TEX_SWORD];
	X=Man->X+Man->DirX*0.75f;
	Y=Man->Y;
	Type=1;
}
bool CSword::Move(const CInputState* is) {
	float vangle=0.02f;
	float max_angle=0.5f;
	Angle+=vangle*Man->DirX;
	float rad=Angle*D3DX_PI*2;
	X=Man->X+Man->DirX*0.25f+sin(rad)*0.5f;
	Y=Man->Y-cos(rad)*0.5f;
	if (abs(Angle)>=max_angle) {
		Man->Sword=false;
		return false;
	}
	return true;
}

// 스테이지
class CSwordStage : public CStage {
public:
	CSwordStage() : CStage("SWORD") {}
	virtual void Init() {
		new CSwordMan();
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 채찍

// 채찍
class CWhipMan;
class CWhip : public CMover {
public:
	int State, Time;
	CWhipMan* Man;
	CWhip(CWhipMan* man);
	virtual bool Move(const CInputState* is);
};

// 사람
class CWhipMan : public CMover {
public:
	float VX, DirX;
	bool Whip;

	CWhipMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		Whip=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;

		if (!Whip) {
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

			if (is->Button[0]) {
				new CWhip(this);
				Whip=true;
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 채찍
CWhip::CWhip(CWhipMan* man) {
	Man=man;
	Texture=Game->Texture[TEX_FILL];
	X=Man->X+Man->DirX*0.75f;
	Y=Man->Y;
	Type=1;
	W=0;
	H=0.2f;
	State=0;
}
bool CWhip::Move(const CInputState* is) {
	float vlength=0.4f;
	float max_length=4;
	switch (State) {
		case 0:
			W+=vlength;
			if (W>=max_length) {
				Time=10;
				State=1;
			}
			break;
		case 1:
			Time--;
			if (Time==0) State=2;
			break;
		case 2:
			W-=vlength;
			if (W<=0) {
				Man->Whip=false;
				return false;
			}
			break;
	}
	X=Man->X+Man->DirX*(W*0.5f+0.3f);
	Y=Man->Y;
	return true;
}

// 스테이지
class CWhipStage : public CStage {
public:
	CWhipStage() : CStage("WHIP") {}
	virtual void Init() {
		new CWhipMan();
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 공

// 공
class CBouncingBall : public CMover {
public:
	float VX, VY;
	CBouncingBall(float x, float y, float vx, float vy) {
		Texture=Game->Texture[TEX_BALL];
		X=x;
		Y=y;
		VX=vx;
		VY=vy;
		Type=1;
	}
	virtual bool Move(const CInputState* is) {
		float max_dist=0.6f;
		X+=VX;
		Y+=VY;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type==2 &&
				abs(X-mover->X)<max_dist &&
				abs(Y-mover->Y)<max_dist
			) {
				VY=-VY;
				Y+=VY;
			}
		}		
		return X>-1 && X<MAX_X;
	}
};

// 사람
class CBouncingBallMan : public CMover {
public:
	float VX, DirX;
	bool PrevButton;

	CBouncingBallMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float throw_speed=0.2f;

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

		if (!PrevButton && is->Button[0]) {
			new CBouncingBall(X, Y, DirX*throw_speed, -throw_speed);
		}
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CBouncingBallStage : public CStage {
public:
	CBouncingBallStage() : CStage("BOUNCING BALL") {}
	virtual void Init() {
		new CBouncingBallMan();
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, MAX_Y-1, 2);
			new CStaticFloor(i, MAX_Y-5, 2);
		}
	}
};


//==============================================================
// 수류탄

// 폭발
class CGrenadeExplosion : public CMover {
public:
	CGrenadeExplosion(float x, float y) {
		Texture=Game->Texture[TEX_FIRE];
		X=x;
		Y=y;
		Type=1;
	}
	virtual bool Move(const CInputState* is) {
		float vsize=0.2f;
		float max_size=4;
		Y-=vsize*0.5f;
		W+=vsize;
		H+=vsize;
		return W<max_size;
	}
};

// 수류탄
class CGrenade : public CMover {
public:
	float VX, VY;
	CGrenade(float x, float y, float vx, float vy) {
		Texture=Game->Texture[TEX_BOMB];
		X=x;
		Y=y;
		VX=vx;
		VY=vy;
	}
	virtual bool Move(const CInputState* is) {
		float accel=0.02f;
		X+=VX;
		VY+=accel;
		Y+=VY;
		if (Y>=MAX_Y-2) {
			new CGrenadeExplosion(X, Y);
			return false;
		}
		return X>-1 && X<MAX_X;
	}
};

// 사람
class CGrenadeMan : public CMover {
public:
	float VX, DirX;
	bool PrevButton;

	CGrenadeMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		PrevButton=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float throw_speed=0.2f;

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

		if (!PrevButton && is->Button[0]) {
			new CGrenade(X, Y, DirX*throw_speed, -throw_speed*2);
		}
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CGrenadeStage : public CStage {
public:
	CGrenadeStage() : CStage("GRENADE") {}
	virtual void Init() {
		new CGrenadeMan();
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, MAX_Y-1, 2);
		}
	}
};


//==============================================================
// 시한폭탄

// 시한폭탄
class CTimeBomb : public CMover {
public:
	int Time;
	CTimeBomb(float x, float y) {
		Texture=Game->Texture[TEX_BOMB];
		X=x;
		Y=y;
		Time=90;
	}
	virtual bool Move(const CInputState* is) {
		Time--;
		if (Time==0) {
			new CGrenadeExplosion(X, Y);
			return false;
		}
		return true;
	}
};

// 사람
class CTimeBombMan : public CMover {
public:
	float VX;
	bool PrevButton;

	CTimeBombMan() {
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
			new CTimeBomb(X, Y);
		}
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CTimeBombStage : public CStage {
public:
	CTimeBombStage() : CStage("TIME BOMB") {}
	virtual void Init() {
		new CTimeBombMan();
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, MAX_Y-1, 2);
		}
	}
};


//==============================================================
// 폭연

// 폭탄
class CBlastBomb : public CMover {
public:
	int Time, BlastCount;
	CBlastBomb(float x, float y) {
		Texture=Game->Texture[TEX_BOMB];
		X=(int)(x+0.5f);
		Y=(int)(y+0.5f);
		Time=90;
		BlastCount=0;
	}
	virtual bool Move(const CInputState* is);
};

// 폭연
class CBlast : public CMover {
public:
	CBlastBomb* Bomb;
	int State;
	CBlast(float x, float y, CBlastBomb* bomb) {
		Texture=Game->Texture[TEX_BLAST];
		X=x;
		Y=y;
		W=H=0;
		Bomb=bomb;
		State=0;
		Type=1;
		Bomb->BlastCount++;
	}
	bool Hit(float x, float y) {
		float max_dist=0.5f;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (
				mover->Type!=0 &&
				abs(x-mover->X)<max_dist &&
				abs(y-mover->Y)<max_dist
			) {
				return true;
			}
		}
		return false;
	}
	virtual bool Move(const CInputState* is) {
		float vsize=0.1f;
		float max_size=1.0f;
		int max_blast=20;
		switch (State) {
			case 0:
				W+=vsize;
				H+=vsize;
				if (W>=max_size) {
					for (int i=-1; i<=1; i+=2) {
						if (Bomb->BlastCount<max_blast && !Hit(X+i, Y)) {
							new CBlast(X+i, Y, Bomb);
						}
						if (Bomb->BlastCount<max_blast && !Hit(X, Y+i)) {
							new CBlast(X, Y+i, Bomb);
						}
					}
					State=1;
				}
				break;
			case 1:
				W-=vsize*0.1f;
				H-=vsize*0.1f;
				if (W<=0) {
					Bomb->BlastCount--;
					return false;
				}
				break;
		}
		return true;
	}
};

// 폭탄
bool CBlastBomb::Move(const CInputState* is) {
	Time--;
	if (Time==0) {
		new CBlast(X, Y, this);
		W=H=0;
	}
	return Time>=0 || BlastCount>0;
}

// 사람
class CBlastMan : public CMover {
public:
	float VX;
	bool PrevButton;

	CBlastMan() {
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
			new CBlastBomb(X, Y);
		}
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CBlastStage : public CStage {
public:
	CBlastStage() : CStage("BLAST") {}
	virtual void Init() {
		new CBlastMan();
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, MAX_Y-1, 2);
			for (int j=3; j<=7; j++) {
				if (Game->Rand.Real1()<0.3f) new CStaticFloor(i, MAX_Y-j, 2);
			}
		}
	}
};


//==============================================================
// 기관총

// 총알
class CMachineGunBullet : public CMover {
public:
	float VX, Dist;
	CMachineGunBullet(float x, float y, float vx) {
		Texture=Game->Texture[TEX_BULLET];
		X=x;
		Y=y;
		VX=vx;
		ReverseX=(vx<0);
		Type=1;
		Dist=0;
	}
	virtual bool Move(const CInputState* is) {
		float max_dist=4;
		X+=VX;
		Dist+=abs(VX);
		return Dist<max_dist;
	}
};

// 사람
class CMachineGunMan : public CMover {
public:
	float VX, DirX;
	int Time;

	CMachineGunMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		Time=0;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float bullet_speed=0.2f;
		int wait_time=5;

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

		if (Time>0) Time--;
		if (Time==0 && is->Button[0]) {
			new CMachineGunBullet(X, Y, DirX*bullet_speed);
			Time=wait_time;
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CMachineGunStage : public CStage {
public:
	CMachineGunStage() : CStage("MACHINE GUN") {}
	virtual void Init() {
		new CMachineGunMan();
		for (int i=0; i<3; i++) new CWeaponEnemy(true);
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 유도 미사일

// 유도 미사일
class CHomingMissile : public CMover {
public:
	float Rad;
	CHomingMissile(float x, float y, float rad) {
		Texture=Game->Texture[TEX_MISSILE];
		X=x;
		Y=y;
		Rad=rad;
	}
	virtual bool Move(const CInputState* is) {
		float vrad=0.08f;
		float speed=0.2f;
		
		float target_dist=MAX_X+MAX_Y;
		CMover* target=NULL;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover=(CMover*)i.Next();
			if (mover->Type==2) {
				float 
					dx=abs(X-mover->X), 
					dy=abs(Y-mover->Y),
					dist=sqrt(dx*dx+dy*dy);
				if (dist<target_dist) {
					target_dist=dist;
					target=mover;
				}
			}
		}
		if (target) {
			float pi2=D3DX_PI*2;
			float diff=atan2f(target->Y-Y, target->X-X)-Rad;
			while (diff<-D3DX_PI) diff+=pi2;
			while (diff>=D3DX_PI) diff-=pi2;
			if (abs(diff)<vrad) {
				Rad+=diff;
			} else {
				Rad+=(diff<0?-vrad:vrad);
			}
		}
		X+=cos(Rad)*speed;
		Y+=sin(Rad)*speed;
		if (Y>=MAX_Y-2) {
			new CGrenadeExplosion(X, Y);
			return false;
		}
		Angle=Rad/D3DX_PI/2+0.25f;
		return X>-1 && X<MAX_X;
	}
};

// 사람
class CHomingMissileMan : public CMover {
public:
	float VX;
	bool PrevButton;

	CHomingMissileMan() {
		Texture=Game->Texture[TEX_MAN];
		X=(MAX_X-1)/2;
		Y=3;
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
			new CHomingMissile(X, Y, -D3DX_PI/2);
		}
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CHomingMissileStage : public CStage {
public:
	CHomingMissileStage() : CStage("HOMING MISSILE") {}
	virtual void Init() {
		new CHomingMissileMan();
		for (int i=0; i<3; i++) {
			CWeaponEnemy* enemy=new CWeaponEnemy();
			enemy->Type=2;
		}
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, MAX_Y-1);
			new CStaticFloor(i, 4);
		}
	}
};


//==============================================================
// 부메랑

// 사람
class CBoomerangMan : public CMover {
public:
	float VX, DirX;
	bool Boomerang;
	CBoomerangMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		Boomerang=false;
	}
	virtual bool Move(const CInputState* is);
};

// 부메랑
class CBoomerang : public CMover {
public:
	float Rad;
	CBoomerangMan* Man;
	int Time;

	CBoomerang(float x, float y, float rad, CBoomerangMan* man) {
		Texture=Game->Texture[TEX_BOOMERANG];
		X=x;
		Y=y;
		Rad=rad;
		Man=man;
		Type=1;
		Time=30;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float max_dist=0.6f;

		if (Time>0) Time--;
		float vrad=(Time==0?0.1f:0.02f);

		float pi2=D3DX_PI*2;
		float diff=atan2f(Man->Y-Y, Man->X-X)-Rad;
		while (diff<-D3DX_PI) diff+=pi2;
		while (diff>=D3DX_PI) diff-=pi2;
		if (abs(diff)<vrad) {
			Rad+=diff;
		} else {
			Rad+=(diff<0?-vrad:vrad);
		}

		X+=cos(Rad)*speed;
		Y+=sin(Rad)*speed;
		Angle=Rad/D3DX_PI/2+0.25f;

		if (
			Time==0 &&
			abs(Man->X-X)<max_dist &&
			abs(Man->Y-Y)<max_dist
		) {
			Man->Boomerang=false;
			return false;
		}
		return true;
	}
};

// 사람
bool CBoomerangMan::Move(const CInputState* is) {
	float speed=0.2f;

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

	if (!Boomerang && is->Button[0]) {
		Boomerang=true;
		new CBoomerang(X, Y, (DirX<0?D3DX_PI*6/5:-D3DX_PI/5), this);
	}

	Angle=VX/speed*0.1f;
	return true;
}

// 스테이지
class CBoomerangStage : public CStage {
public:
	CBoomerangStage() : CStage("BOOMERANG") {}
	virtual void Init() {
		new CBoomerangMan();
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 원격 조종 무기

// 원격 조종 무기
class CRemoteControlWeapon : public CMover {
public:
	float VX;

	CRemoteControlWeapon(float x, float y) {
		Texture=Game->Texture[TEX_SHURIKEN];
		X=x;
		Y=y;
		VX=0;
		Type=1;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float accel=0.04f;
		
		if (is->Left && VX>-speed) VX-=accel;
		if (is->Right && VX<speed) VX+=accel;
		X+=VX;

		Angle+=0.01f;
		return X>-1 && X<MAX_X;
	}
};

// 사람
class CRemoteControlMan : public CMover {
public:
	float VX;
	bool PrevButton;
	CRemoteControlMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		PrevButton=false;
	}
	virtual bool Move(const CInputState* is) {
		float speed=0.1f;

		VX=0;
		if (is->Left) VX=-speed;
		if (is->Right) VX=speed;

		X+=VX;
		if (X<0) X=0;
		if (X>MAX_X-1) X=MAX_X-1;

		if (!PrevButton && is->Button[0]) {
			new CRemoteControlWeapon(X, Y);
		}
		PrevButton=is->Button[0];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CRemoteControlStage : public CStage {
public:
	CRemoteControlStage() : CStage("REMOTE CONTROL") {}
	virtual void Init() {
		new CRemoteControlMan();
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 방패

// 방패
class CShield : public CMover {
public:
	CShield() {
		Texture=Game->Texture[TEX_SHIELD];
		Type=1;
	}	
};

// 사람
class CShieldMan : public CMover {
public:
	float VX, DirX;
	CShield* Shield;

	CShieldMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		Shield=NULL;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float shield_x=0.6f;

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

		if (!Shield && is->Button[0]) Shield=new CShield();

		if (Shield) {
			Shield->X=X+DirX*shield_x;
			Shield->Y=Y;
			if (!is->Button[0]) {
				delete Shield;
				Shield=NULL;
			}
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CShieldStage : public CStage {
public:
	CShieldStage() : CStage("SHIELD") {}
	virtual void Init() {
		new CShieldMan();
		for (int i=0; i<3; i++) {
			CWeaponEnemy* enemy=new CWeaponEnemy();
			enemy->Texture=Game->Texture[TEX_BULLET];
			enemy->ReverseX=true;
		}
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 연막

// 연막
class CSmoke : public CMover {
public:
	int State;
	CSmoke(float x, float y) {
		Texture=Game->Texture[TEX_BLAST];
		X=x;
		Y=y;
		W=H=0;
		State=0;
		Type=1;
	}
	virtual bool Move(const CInputState* is) {
		float vsize=0.1f;
		float max_size=1.0f;
		switch (State) {
			case 0:
				W+=vsize;
				H+=vsize;
				if (W>=max_size) State=1;
				break;
			case 1:
				W-=vsize*0.2f;
				H-=vsize*0.2f;
				if (W<=0) return false;
				break;
		}
		return true;
	}
};

// 사람
class CSmokeMan : public CMover {
public:
	float VX;

	CSmokeMan() {
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

		if (is->Button[0]) {
			bool smoke=false;
			float x=(int)(X+0.5f), y=(int)(Y+0.5f);
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				CMover* mover=(CMover*)i.Next();
				if (
					mover->Type==1 &&
					abs(x-mover->X)<max_dist &&
					abs(y-mover->Y)<max_dist
				) {
					smoke=true;
					break;
				}
			}
			if (!smoke) new CSmoke(x, y);
		}

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CSmokeStage : public CStage {
public:
	CSmokeStage() : CStage("SMOKE") {}
	virtual void Init() {
		new CSmokeMan();
		for (int i=0; i<3; i++) new CWeaponEnemy();
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, MAX_Y-1, 2);
		}
	}
};


//==============================================================
// 거품

// 거품
class CBubble : public CMover {
public:
	float VX;
	int State;
	CMover* Man;
	CBubble(float x, float y, float vx, CMover* man) {
		Texture=Game->Texture[TEX_BUBBLE];
		X=x;
		Y=y;
		VX=vx;
		Man=man;
		State=0;
	}
	virtual bool Move(const CInputState* is) {
		float max_dist=0.6f;
		float bubble_vy=-0.05f;
		float vsize=0.1f;
		switch (State) {
			case 0:
				X+=VX;
				for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
					CMover* mover=(CMover*)i.Next();
					if (
						mover->Type==2 &&
						abs(X-mover->X)<max_dist &&
						abs(Y-mover->Y)<max_dist
					) {
						((CWeaponEnemy*)mover)->Init();
						State=1;
					}
				}
				if (X<-1 || X>MAX_X) return false;
				break;
			case 1:
				Y+=bubble_vy;
				if (
					abs(X-Man->X)<max_dist &&
					abs(Y-Man->Y)<max_dist
				) {
					State=2;
				}
				if (Y<-1) return false;
				break;
			case 2:
				W-=vsize;
				H-=vsize;
				if (W<=0) return false;
				break;
		}
		return true;
	}
	virtual void Draw() {
		float w=W, h=H;
		if (State!=0) {
			W*=0.7f;
			H*=0.7f;
			Texture=Game->Texture[TEX_ENEMY0];
			CMover::Draw();
			Texture=Game->Texture[TEX_BUBBLE];
			W=w;
			H=h;
		}
		CMover::Draw();
	}
};

// 사람
class CBubbleMan : public CMover {
public:
	float VX, DirX, VY;
	bool PrevButton, Jump;

	CBubbleMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=VY=0;
		DirX=1;
		PrevButton=Jump=false;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float bubble_speed=0.2f;
		float jump_speed=-0.5f;
		float jump_accel=0.02f;

		if (!Jump) {
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
				new CBubble(X, Y, DirX*bubble_speed, this);
			}
			PrevButton=is->Button[0];
			if (is->Button[1]) {
				Jump=true;
				VY=jump_speed;
			}
		} else
		{
			VY+=jump_accel;
			Y+=VY;
			if (VY>0 && Y>=MAX_Y-2) {
				Y=MAX_Y-2;
				Jump=false;
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
class CBubbleStage : public CStage {
public:
	CBubbleStage() : CStage("BUBBLE") {}
	virtual void Init() {
		new CBubbleMan();
		for (int i=0; i<3; i++) {
			CWeaponEnemy* enemy=new CWeaponEnemy(true);
			enemy->Texture=Game->Texture[TEX_ENEMY0];
			enemy->Type=2;
		}
		for (int i=0; i<MAX_X; i++) new CStaticFloor(i, MAX_Y-1);
	}
};


//==============================================================
// 무기 교체

// 사람
class CChangeWeaponMan : public CMover {
public:
	float VX, DirX;
	bool PrevButton[2];
	int Weapon;

	CChangeWeaponMan() {
		Texture=Game->Texture[TEX_MAN];
		X=3;
		Y=MAX_Y-2;
		VX=0;
		DirX=1;
		PrevButton[0]=PrevButton[1]=false;
		Weapon=0;
	}

	virtual bool Move(const CInputState* is) {
		float speed=0.2f;
		float weapon_speed=0.2f;
		int weapon_count=4;

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

		if (!PrevButton[0] && is->Button[0]) {
			switch (Weapon) {
				case 0:
					new CGrenade(X, Y, DirX*weapon_speed, -weapon_speed*2);
					break;
				case 1:
					new CMachineGunBullet(X, Y, DirX*weapon_speed);
					break;
				case 2:
					new CHomingMissile(X, Y, -D3DX_PI/2);
					break;
				case 3:
					new CBubble(X, Y, DirX*weapon_speed, this);
					break;
			}
		}
		PrevButton[0]=is->Button[0];

		if (!PrevButton[1] && is->Button[1]) {
			Weapon=(Weapon+1)%weapon_count;
		}
		PrevButton[1]=is->Button[1];

		Angle=VX/speed*0.1f;
		return true;
	}
};

// 스테이지
class CChangeWeaponStage : public CStage {
public:
	CChangeWeaponMan* Man;
	CChangeWeaponStage() : CStage("SWITCHING WEAPON") {}
	virtual void Init() {
		Man=new CChangeWeaponMan();
		for (int i=0; i<3; i++) {
			CWeaponEnemy* enemy=new CWeaponEnemy(true);
			enemy->Texture=Game->Texture[TEX_ENEMY0];
			enemy->Type=2;
		}
		for (int i=0; i<MAX_X; i++) {
			new CStaticFloor(i, MAX_Y-1, 2);
		}
	}
	virtual void Draw() {
		float sw=Game->GetGraphics()->GetWidth()/MAX_X;
		float sh=Game->GetGraphics()->GetHeight()/MAX_Y;
		int texture[]={TEX_BOMB, TEX_BULLET, TEX_MISSILE, TEX_BUBBLE};
		for (int i=0; i<4; i++) {
			float x=sw*(i*3+3), y=sh*2;
			D3DCOLOR color=(Man->Weapon==i?COL_BLACK:COL_LGRAY);
			Game->Texture[texture[i]]->Draw(
				x, y, color, 0, 0,
				x+sw, y, color, 1, 0,
				x, y+sh, color, 0, 1,
				x+sw, y+sh, color, 1, 1
			);
		}
	}
};

