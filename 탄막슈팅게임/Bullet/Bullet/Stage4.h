//==============================================================
class CNWayShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRange;
	float ShotSpeed;
	int ShotCount;
	int Interval;
	int Time;
	float BulletAngleRate;
	float BulletSpeedRate;

	CNWayShooter(
		float angle, float angle_range, float speed, int count, int interval, 
		float bullet_angle_rate, float bullet_speed_rate)
	:	CEnemy(PsEnemy),
		ShotAngle(angle), 
		ShotAngleRange(angle_range), 
		ShotSpeed(speed),
		ShotCount(count), 
		Interval(interval), 
		BulletAngleRate(bullet_angle_rate),
		BulletSpeedRate(bullet_speed_rate),
		Time(0)
	{}

	void Move() {
		if (Time==0) {
			ShootNWay(PsNeedle, X, Y, ShotAngle, ShotAngleRange, ShotSpeed, ShotCount, BulletAngleRate, BulletSpeedRate);
		}
		Time=(Time+1)%Interval;
	}
};

void NWay() {
	new CNWayShooter(0.25f, 0.2f, 0.02f, 7, 5, 0, 0);
}

void NWay2() {
	new CNWayShooter(0.25f, 0.2f, 0.02f, 8, 5, 0, 0);
}

void NWay3() {
	new CNWayShooter(0.25f, 1, 0.02f, 8, 5, 0, 0);
}

//==============================================================
class CCircleShooter : public CNWayShooter {
public:
	CCircleShooter(
		float angle, float speed, int count, int interval, 
		float bullet_angle_rate, float bullet_speed_rate)
	:	CNWayShooter(angle, 1.0f-1.0f/count, speed, count, interval, bullet_angle_rate, bullet_speed_rate)
	{}
};

void Circle() {
	new CCircleShooter(0.25f, 0.02f, 8, 5, 0, 0);
}

void Circle2() {
	new CCircleShooter(0.25f, 0.02f, 32, 5, 0, 0);
}

void Circle3() {
	new CCircleShooter(0.234375f, 0.02f, 32, 5, 0, 0);
}

void BentCircle() {
	new CCircleShooter(0.25f, 0.02f, 32, 5, 0.002f, 0);
}

void BentCircle2() {
	new CCircleShooter(0.25f, 0, 32, 5, 0, 0.0005f);
}

void BentCircle3() {
	new CCircleShooter(0.25f, 0, 32, 5, 0.002f, 0.0005f);
}

//==============================================================
void DenseNWay() {
	new CNWayShooter(0.25f, 0.5f, 0.02f, 16, 5, 0, 0);	
}

void DenseNWay2() {
	new CNWayShooter(0.25f, 0.5f, 0.02f, 32, 5, 0, 0);	
}

void DenseNWay3() {
	new CNWayShooter(0.25f, 0.5f, 0.02f, 64, 5, 0, 0);	
}

void DenseNWay4() {
	new CNWayShooter(0.25f, 0.5f, 0.02f, 128, 5, 0, 0);	
}

void DenseNWay5() {
	new CNWayShooter(0.25f, 0.5f, 0.02f, 256, 5, 0, 0);	
}

//==============================================================
class CAimingNWayShooter : public CEnemy {
public:
	float ShotAngleRange;
	float ShotSpeed;
	int ShotCount;
	int Interval;
	int Time;

	CAimingNWayShooter(float angle_range, float speed, int count, int interval)
	:	CEnemy(PsEnemy),
		ShotAngleRange(angle_range), 
		ShotSpeed(speed),
		ShotCount(count), 
		Interval(interval), 
		Time(0)
	{}

	void Move() {
		if (Time==0) {
			ShootNWay(PsNeedle, X, Y, GetMyShipAngle(X, Y), ShotAngleRange, ShotSpeed, ShotCount, 0, 0);
		}
		Time=(Time+1)%Interval;
	}
};

void AimingNWay() {
	new CAimingNWayShooter(0.25f, 0.02f, 9, 10);
}

void AimingNWay2() {
	new CAimingNWayShooter(0.25f, 0.02f, 10, 10);
}

void AimingNWay3() {
	CMover* mover=new CAimingNWayShooter(0.25f, 0.04f, 9, 2);
	mover->Y=-0.2f;
}

//==============================================================
class CIntermittentAimingNWayShooter : public CEnemy {
public:
	float ShotAngleRange;
	float ShotSpeed;
	int ShotCount;
	int Interval;
	int ShotTime;
	int WaitTime;
	int Time;

	CIntermittentAimingNWayShooter(
		float angle_range, float speed, int count, int interval, 
		int shot_time, int wait_time)
	:	CEnemy(PsEnemy),
		ShotAngleRange(angle_range), 
		ShotSpeed(speed),
		ShotCount(count), 
		Interval(interval), 
		ShotTime(shot_time),
		WaitTime(wait_time),
		Time(0)
	{}

	void Move() {
		if (Time<ShotTime && Time%Interval==0) {
			ShootNWay(PsNeedle, X, Y, GetMyShipAngle(X, Y), ShotAngleRange, ShotSpeed, ShotCount, 0, 0);
		}
		Time=(Time+1)%(ShotTime+WaitTime);
	}
};

void IntermittentAimingNWay() {
	new CIntermittentAimingNWayShooter(0.25f, 0.04f, 9, 2, 60, 20);
}

//==============================================================
class CRandomNWayShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRange;
	float ShotSpeed;
	int ShotCount;
	int Interval;
	int Time;

	CRandomNWayShooter(float angle, float angle_range, float speed, int count, int interval)
	:	CEnemy(PsEnemy),
		ShotAngle(angle), 
		ShotAngleRange(angle_range), 
		ShotSpeed(speed),
		ShotCount(count),
		Interval(interval), 
		Time(0)
	{}

	void Move() {
		if (Time==0) {
			for (int i=0; i<ShotCount; i++) {
				new CBullet(
					PsNeedle, X, Y, 
					ShotAngle+ShotAngleRange*(Rand.Real1()-0.5f), 
					0, ShotSpeed, 0);
			}
		}
		Time=(Time+1)%Interval;
	}
};

void RandomNWay() {
	new CRandomNWayShooter(0.25f, 0.2f, 0.02f, 1, 1);
}

void RandomNWay2() {
	new CRandomNWayShooter(0.25f, 0.2f, 0.02f, 10, 10);
}

void RandomNWay3() {
	new CRandomNWayShooter(0.25f, 0.2f, 0.02f, 3, 1);
}

void RandomNWay4() {
	new CRandomNWayShooter(0.25f, 0.2f, 0.02f, 1, 3);
}

//==============================================================
class CRandomCircleShooter : public CEnemy {
public:
	float ShotSpeed;
	int ShotCount;
	int Interval;
	int Time;

	CRandomCircleShooter(float speed, int count, int interval)
	:	CEnemy(PsEnemy),
		ShotSpeed(speed),
		ShotCount(count),
		Interval(interval), 
		Time(0)
	{}

	void Move() {
		if (Time==0) {
			for (int i=0; i<ShotCount; i++) {
				new CBullet(PsNeedle, X, Y, Rand.Real2(), 0, ShotSpeed, 0);
			}
		}
		Time=(Time+1)%Interval;
	}
};

void RandomCircle() {
	new CRandomCircleShooter(0.02f, 3, 1);
}

//==============================================================
class CRollingNWayShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRange;
	float ShotAngleRate;
	float ShotSpeed;
	int ShotCount;
	int NWayCount;
	int Interval;
	int Time;

	CRollingNWayShooter(
		float angle, float angle_range, float angle_rate, 
		float speed, int count, int nway_count, int interval)
	:	CEnemy(PsEnemy),
		ShotAngle(angle), 
		ShotAngleRange(angle_range), 
		ShotAngleRate(angle_rate), 
		ShotSpeed(speed),
		ShotCount(count),
		NWayCount(nway_count),
		Interval(interval), 
		Time(0)
	{}

	void Move() {
		if (Time==0) {
			for (int i=0; i<NWayCount; i++) {
				ShootNWay(
					PsNeedle, X, Y, 
					ShotAngle+(float)i/NWayCount, ShotAngleRange, 
					ShotSpeed, ShotCount, 0, 0);
			}
			ShotAngle+=ShotAngleRate;
			ShotAngle-=floor(ShotAngle);
		}
		Time=(Time+1)%Interval;
	}
};

void RollingNWay() {
	new CRollingNWayShooter(0.25f, 0.2f, 0.01f, 0.02f, 5, 1, 5);
}

void RollingNWay2() {
	new CRollingNWayShooter(0.25f, 0.2f, 0.01f, 0.02f, 5, 3, 5);
}

void RollingNWay3() {
	new CRollingNWayShooter(0.25f, 0.9375f, 0.01f, 0.02f, 16, 1, 5);
}

void RollingNWay4() {
	CMover* mover=new CRollingNWayShooter(0.125f, 0.2f, 0.01f, 0.02f, 10, 4, 5);
	mover->Y=-0.3f;
}

//==============================================================
class CWavingNWayShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRange;
	float WavingAngleRange;
	float ShotSpeed;
	int ShotCount;
	int Interval;
	int Cycle;
	int Time;

	CWavingNWayShooter(
		float angle, float angle_range, 
		float waving_angle_range, float speed, 
		int count, int interval, int cycle)
	:	CEnemy(PsEnemy),
		ShotAngle(angle), 
		ShotAngleRange(angle_range), 
		WavingAngleRange(waving_angle_range), 
		ShotSpeed(speed),
		ShotCount(count),
		Interval(interval), 
		Cycle(cycle),
		Time(0)
	{}

	void Move() {
		if (Time%Interval==0) {
			ShootNWay(
				PsNeedle, X, Y, 
				ShotAngle+WavingAngleRange*sinf(D3DX_PI*2*Time/Cycle),
				ShotAngleRange, 
				ShotSpeed, ShotCount, 0, 0);
		}
		Time=(Time+1)%Cycle;
	}
};

void WavingNWay() {
	new CWavingNWayShooter(0.25f, 0.2f, 0.05f, 0.02f, 5, 5, 120);
}

void WavingNWay2() {
	new CWavingNWayShooter(0.25f, 0.2f, 0.05f, 0.02f, 5, 1, 120);
}

//==============================================================
class CWavingCircleShooter : public CWavingNWayShooter {
public:
	CWavingCircleShooter(
		float angle, float waving_angle_range, 
		float speed, int count, int interval, int cycle)
	:	CWavingNWayShooter(
			angle, 1.0f-1.0f/count, waving_angle_range, 
			speed, count, interval, cycle)
	{}
};

void WavingCircle() {
	new CWavingCircleShooter(0.25f, 0.1f, 0.02f, 8, 1, 120);
}

