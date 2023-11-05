//==============================================================
class CDirectionalShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotSpeed;

	CDirectionalShooter(float angle, float speed) 
	:	CEnemy(DpEnemy),
		ShotAngle(angle),
		ShotSpeed(speed)
	{}

	void Move() {
		new CBullet(DpRedBullet, X, Y, ShotAngle, 0, ShotSpeed, 0);
	}
};

void Directional() {
	new CDirectionalShooter(0.25f, 0.1f);
}

void Directional2() {
	new CDirectionalShooter(0.5f, 0.1f);
}

void Directional3() {
	new CDirectionalShooter(0.375f, 0.1f);
}

//==============================================================
class CSpiralShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRate;
	float ShotSpeed;

	CSpiralShooter(float angle, float angle_rate, float speed) 
	:	CEnemy(DpEnemy),
		ShotAngle(angle),
		ShotAngleRate(angle_rate), 
		ShotSpeed(speed)
	{}

	void Move() {
		new CBullet(DpRedBullet, X, Y, ShotAngle, 0, ShotSpeed, 0);
		ShotAngle+=ShotAngleRate;
		ShotAngle-=floor(ShotAngle);
	}
};

void Spiral() {
	new CSpiralShooter(0, 0.02f, 0.01f);
}

void Spiral2() {
	new CSpiralShooter(0, 0.01f, 0.01f);
}

void Spiral3() {
	new CSpiralShooter(0, 0.03f, 0.01f);
}

void Spiral4() {
	new CSpiralShooter(0, 0.02f, 0.005f);
}

void Spiral5() {
	new CSpiralShooter(0, 0.02f, 0.02f);
}

//==============================================================
class CMultipleSpiralShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRate;
	float ShotSpeed;
	int ShotCount;

	CMultipleSpiralShooter(float angle, float angle_rate, float speed, int count) 
	:	CEnemy(DpEnemy),
		ShotAngle(angle), 
		ShotAngleRate(angle_rate), 
		ShotSpeed(speed),
		ShotCount(count)
	{}

	void Move() {
		for (int i=0; i<ShotCount; i++) {
			new CBullet(DpRedBullet, X, Y, ShotAngle+(float)i/ShotCount, 0, ShotSpeed, 0);
		}
		ShotAngle+=ShotAngleRate;
		ShotAngle-=floor(ShotAngle);
	}
};

void MultipleSpiral() {
	new CMultipleSpiralShooter(0, 0.02f, 0.01f, 4);
}

//==============================================================
class CIntervalMultipleSpiralShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRate;
	float ShotSpeed;
	int ShotCount;
	int Interval;
	int Time;

	CIntervalMultipleSpiralShooter(float angle, float angle_rate, float speed, int count, int interval) 
	:	CEnemy(DpEnemy),
		ShotAngle(angle), 
		ShotAngleRate(angle_rate), 
		ShotSpeed(speed),
		ShotCount(count), 
		Interval(interval), 
		Time(0)
	{}

	void Move() {
		if (Time==0) {
			for (int i=0; i<ShotCount; i++) {
				new CBullet(DpRedBullet, X, Y, ShotAngle+(float)i/ShotCount, 0, ShotSpeed, 0);
			}
			ShotAngle+=ShotAngleRate;
			ShotAngle-=floor(ShotAngle);
		}
		Time=(Time+1)%Interval;
	}
};

void IntervalMultipleSpiral() {
	new CIntervalMultipleSpiralShooter(0, 0.02f, 0.01f, 4, 5);
}

void IntervalMultipleSpiral2() {
	new CIntervalMultipleSpiralShooter(0, -0.02f, 0.01f, 4, 5);
}

//==============================================================
class CBiDirectionalSpiralShooter : public CEnemy {
public:
	float ShotAngle[2];
	float ShotAngleRate[2];
	float ShotSpeed;
	int ShotCount;
	int Interval;
	int Time;

	CBiDirectionalSpiralShooter(float angle, float angle_rate0, float angle_rate1, float speed, int count, int interval) 
	:	CEnemy(DpEnemy),
		ShotSpeed(speed),
		ShotCount(count),
		Interval(interval), 
		Time(0)
	{
		ShotAngle[0]=angle;
		ShotAngle[1]=angle;
		ShotAngleRate[0]=angle_rate0;
		ShotAngleRate[1]=angle_rate1;
	}

	void Move() {
		if (Time==0) {
			for (int j=0; j<2; j++) {
				for (int i=0; i<ShotCount; i++) {
					new CBullet(DpRedBullet, X, Y, ShotAngle[j]+(float)i/ShotCount, 0, ShotSpeed, 0);
				}
				ShotAngle[j]+=ShotAngleRate[j];
				ShotAngle[j]-=floor(ShotAngle[j]);
			}
		}
		Time=(Time+1)%Interval;
	}
};

void BiDirectionalSpiral() {
	new CBiDirectionalSpiralShooter(0, 0.02f, -0.02f, 0.01f, 4, 5);
}

void BiDirectionalSpiral2() {
	new CBiDirectionalSpiralShooter(0, 0.03f, -0.02f, 0.01f, 4, 5);
}

void BiDirectionalSpiral3() {
	new CBiDirectionalSpiralShooter(0, 0.03f, -0.02f, 0.02f, 4, 5);
}

void BiDirectionalSpiral4() {
	new CBiDirectionalSpiralShooter(0, 0.015f, -0.01f, 0.02f, 4, 5);
}

//==============================================================
class CBentSpiralShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRate;
	float ShotSpeed;
	int ShotCount;
	int Interval;
	int Time;
	float BulletAngleRate;
	float BulletSpeedRate;

	CBentSpiralShooter(
		float angle, float angle_rate, float speed, int count, int interval, 
		float bullet_angle_rate, float bullet_speed_rate
	) 
	:	CEnemy(DpEnemy),
		ShotAngle(angle),
		ShotAngleRate(angle_rate), 
		ShotSpeed(speed),
		ShotCount(count),
		Interval(interval), 
		Time(0),
		BulletAngleRate(bullet_angle_rate),
		BulletSpeedRate(bullet_speed_rate)
	{}

	void Move() {
		if (Time==0) {
			for (int i=0; i<ShotCount; i++) {
				new CBullet(
					DpBlueBullet, X, Y, 
					ShotAngle+(float)i/ShotCount, BulletAngleRate,
					ShotSpeed, BulletSpeedRate);
			}
			ShotAngle+=ShotAngleRate;
			ShotAngle-=floor(ShotAngle);
		}
		Time=(Time+1)%Interval;
	}
};

void BentSpiral() {
	new CBentSpiralShooter(0, 0.02f, 0.005f, 1, 10, 0, 0);
}

void BentSpiral2() {
	new CBentSpiralShooter(0, 0.02f, 0.005f, 1, 10, -0.003f, 0);
}

void BentSpiral3() {
	new CBentSpiralShooter(0, 0.02f, 0.005f, 1, 10, 0, 0.0002f);
}

void BentSpiral4() {
	new CBentSpiralShooter(0, 0.02f, 0.005f, 1, 10, -0.003f, 0.0002f);
}

//==============================================================
class CCombinedSpiralShooter : public CEnemy {
public:
	CBiDirectionalSpiralShooter* BiDirectional;
	CBentSpiralShooter* Bent;

	CCombinedSpiralShooter()
	:	CEnemy(DpEnemy)
	{
		BiDirectional=new CBiDirectionalSpiralShooter(0, 0.015f, -0.01f, 0.02f, 4, 5);
		Bent=new CBentSpiralShooter(0, 0.02f, 0, 10, 10, -0.003f, 0.0002f);
		BiDirectional->SetShape(ShVoid);
		Bent->SetShape(ShVoid);
	}
};

void CombinedSpiral() {
	new CCombinedSpiralShooter();
}

//==============================================================
class CWasherSpiralShooter : public CEnemy {
public:
	CBiDirectionalSpiralShooter* BiDirectional;
	CBentSpiralShooter* Bent;
	float MaxShotAngleRate;
	float MaxBulletAngleRate;
	int Time;

	CWasherSpiralShooter()
	:	CEnemy(DpEnemy),
		MaxShotAngleRate(0.02f), 
		MaxBulletAngleRate(0.003f),
		Time(0)
	{
		BiDirectional=new CBiDirectionalSpiralShooter(0, 0.015f, -0.01f, 0.02f, 4, 5);
		Bent=new CBentSpiralShooter(0, 0, 0, 10, 10, 0, 0.0002f);
		BiDirectional->SetShape(ShVoid);
		Bent->SetShape(ShVoid);
	}
	
	void Move() {
		if (Time<250) {
			Bent->ShotAngleRate=MaxShotAngleRate;
			Bent->BulletAngleRate=-MaxBulletAngleRate;
		} else
		if (Time<300) {
			Bent->ShotAngleRate=MaxShotAngleRate*(275-Time)/25;
			Bent->BulletAngleRate=-MaxBulletAngleRate*(275-Time)/25;
		} else
		if (Time<550) {
			Bent->ShotAngleRate=-MaxShotAngleRate;
			Bent->BulletAngleRate=MaxBulletAngleRate;
		} else {
			Bent->ShotAngleRate=-MaxShotAngleRate*(575-Time)/25;
			Bent->BulletAngleRate=MaxBulletAngleRate*(575-Time)/25;
		}
		Time=(Time+1)%600;
	}
};

void WasherSpiral() {
	new CWasherSpiralShooter();
}

//==============================================================
class CEasyWasherSpiralShooter : public CWasherSpiralShooter {
public:
	CEasyWasherSpiralShooter() {
		BiDirectional->ShotSpeed=0.01f;
		Bent->BulletSpeedRate=0.0001f;
	}
};

void EasyWasherSpiral() {
	new CEasyWasherSpiralShooter();
}

class CEasyWasherSpiralShooter2 : public CWasherSpiralShooter {
public:
	CEasyWasherSpiralShooter2() {
		BiDirectional->ShotSpeed=0.01f;
		Bent->BulletSpeedRate=0.0001f;
		BiDirectional->Interval=10;
		Bent->Interval=20;
	}
};

void EasyWasherSpiral2() {
	new CEasyWasherSpiralShooter2();
}

class CEasyWasherSpiralShooter3 : public CWasherSpiralShooter {
public:
	CEasyWasherSpiralShooter3() {
		BiDirectional->ShotSpeed=0.01f;
		Bent->BulletSpeedRate=0.0001f;
		BiDirectional->Interval=10;
		Bent->Interval=20;
		MaxShotAngleRate=0.01f;
		MaxBulletAngleRate=0.0015f;
	}
};

void EasyWasherSpiral3() {
	new CEasyWasherSpiralShooter3();
}

class CEasyWasherSpiralShooter4 : public CWasherSpiralShooter {
public:
	CEasyWasherSpiralShooter4() {
		BiDirectional->ShotSpeed=0.01f;
		Bent->BulletSpeedRate=0.0001f;
		BiDirectional->Interval=10;
		Bent->Interval=20;
		MaxShotAngleRate=0.0014f;
		MaxBulletAngleRate=0.0021f;
	}
};

void EasyWasherSpiral4() {
	new CEasyWasherSpiralShooter4();
}

//==============================================================
class CHardWasherSpiralShooter : public CWasherSpiralShooter {
public:
	CHardWasherSpiralShooter() {
		BiDirectional->ShotSpeed=0.04f;
		Bent->BulletSpeedRate=0.0004f;
		BiDirectional->Interval=3;
		Bent->Interval=5;
		MaxShotAngleRate=0.028f;
		MaxBulletAngleRate=0.0042f;
	}
};

void HardWasherSpiral() {
	new CHardWasherSpiralShooter();
}

//==============================================================
class CRankedWasherSpiralShooter : public CWasherSpiralShooter {
public:
	CRankedWasherSpiralShooter(float rank) {
		BiDirectional->ShotSpeed*=rank;
		Bent->BulletSpeedRate*=rank;
		BiDirectional->Interval=max(1, (int)(BiDirectional->Interval/rank+0.5f));
		Bent->Interval=max(1, (int)(Bent->Interval/rank+0.5f));
		MaxShotAngleRate*=sqrtf(rank);
		MaxBulletAngleRate*=sqrtf(rank);
	}
};

void RankedWasherSpiral() {
	new CRankedWasherSpiralShooter(1);
}

void RankedWasherSpiral2() {
	new CRankedWasherSpiralShooter(0.5f);
}

void RankedWasherSpiral3() {
	new CRankedWasherSpiralShooter(2);
}

void RankedWasherSpiral4() {
	new CRankedWasherSpiralShooter(3);
}

