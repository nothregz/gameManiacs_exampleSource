//==============================================================
class CAimingShooter : public CEnemy {
public:
	float ShotSpeed;
	int Interval;
	int Time;

	CAimingShooter(float x, float y, float speed, int interval)
	:	CEnemy(IkEnemy),
		ShotSpeed(speed),
		Interval(interval), 
		Time(0)
	{
		X=x;
		Y=y;
	}

	void Move() {
		if (Time==0) {
			new CBullet(PsBullet, X, Y, GetMyShipAngle(X, Y), 0, ShotSpeed, 0);
		}
		Time=(Time+1)%Interval;
	}
};

void Aiming() {
	new CAimingShooter(0, -0.7f, 0.02f, 10);
}

void Aiming2() {
	new CAimingShooter(-0.4f, -0.7f, 0.02f, 10);
	new CAimingShooter(0.4f, -0.7f, 0.02f, 10);
}

void Aiming3() {
	new CAimingShooter(-0.4f, -0.7f, 0.02f, 10);
	new CAimingShooter(0.4f, -0.7f, 0.03f, 11);
}

void Aiming4() {
	new CAimingShooter(-0.6f, -0.6f, 0.02f, 10);
	new CAimingShooter(-0.3f, -0.7f, 0.03f, 11);
	new CAimingShooter(0, -0.8f, 0.04f, 12);
	new CAimingShooter(0.3f, -0.7f, 0.03f, 11);
	new CAimingShooter(0.6f, -0.6f, 0.02f, 10);
}

//==============================================================
void CircleAndAiming() {
	new CCircleShooter(0.25f, 0.02f, 32, 11, 0, 0);
}

void CircleAndAiming2() {
	new CCircleShooter(0.25f, 0.02f, 32, 11, 0, 0);
	new CAimingShooter(-0.4f, -0.7f, 0.02f, 10);
	new CAimingShooter(0.4f, -0.7f, 0.02f, 10);
}

class CCircleAndAimingShooter : public CEnemy {
public:
	CCircleShooter* Circle;
	CAimingShooter* Aiming;
	
	CCircleAndAimingShooter()
	:	CEnemy(PsEnemy)
	{
		Circle=new CCircleShooter(0.25f, 0.02f, 32, 11, 0, 0);
		Circle->SetShape(ShVoid);
		Aiming=new CAimingShooter(X, Y, 0.03f, 11);
		Aiming->SetShape(ShVoid);
	}
};

void CircleAndAiming3() {
	new CCircleAndAimingShooter();
}

//==============================================================
void Area() {
	CCircleShooter* circle[2];
	circle[0]=new CCircleShooter(0.25f, 0.01f, 11, 15, 0, 0);
	circle[0]->X=-0.6f;
	circle[0]->Y=-0.2f;
	circle[1]=new CCircleShooter(0.25f, 0.01f, 11, 15, 0, 0);
	circle[1]->X=0.6f;
	circle[1]->Y=-0.2f;
}

void Area2() {
	CCircleShooter* circle[2];
	circle[0]=new CCircleShooter(0.25f, 0.04f, 11, 1, 0, 0);
	circle[0]->X=-0.6f;
	circle[0]->Y=-0.2f;
	circle[1]=new CCircleShooter(0.25f, 0.04f, 11, 1, 0, 0);
	circle[1]->X=0.6f;
	circle[1]->Y=-0.2f;
}

class CWinderShooter : public CEnemy {
public:
	CCircleShooter* Circle[2];
	float ShotAngle;
	float WinderAngleRange;
	int Cycle;
	int Time;

	CWinderShooter(
		float angle, float speed, int count, int interval, 
		float winder_angle_range, int cycle
	)
	:	CEnemy(ShVoid),
		ShotAngle(angle), 
		WinderAngleRange(winder_angle_range), 
		Cycle(cycle), Time(0)
	{
		Circle[0]=new CCircleShooter(ShotAngle, speed, count, interval, 0, 0);
		Circle[0]->X=-0.6f;
		Circle[0]->Y=-0.2f;
		Circle[1]=new CCircleShooter(ShotAngle, speed, count, interval, 0, 0);
		Circle[1]->X=0.6f;
		Circle[1]->Y=-0.2f;
	}
	
	void Move() {
		Circle[0]->ShotAngle=ShotAngle-WinderAngleRange*sinf(D3DX_PI*2*Time/Cycle);
		Circle[1]->ShotAngle=ShotAngle+WinderAngleRange*sinf(D3DX_PI*2*Time/Cycle);
		Time=(Time+1)%Cycle;
	}
};

void Winder() {
	new CWinderShooter(0.25f, 0.04f, 11, 1, 0.05f, 300);
}

void Winder2() {
	new CWinderShooter(0.25f, 0.04f, 6, 1, 0.1f, 300);
}

//==============================================================
class CLineShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotSpeed;
	int Interval;
	int ShotTime;
	int WaitTime;
	int Time;

	CLineShooter(
		float x, float y, 
		float angle, float speed, 
		int interval, int shot_time, int wait_time)
	:	CEnemy(PsEnemy),
		ShotAngle(angle), ShotSpeed(speed), 
		Interval(interval), ShotTime(shot_time), 
		WaitTime(wait_time), Time(0)
	{
		X=x;
		Y=y;
	}

	void Move() {
		if (Time<ShotTime && Time%Interval==0) {
			new CBullet(PsNeedle, X, Y, ShotAngle, 0, ShotSpeed, 0);
		}
		Time=(Time+1)%(ShotTime+WaitTime);
	}
};

void Line() {
	new CLineShooter(-0.8f, -0.4f, 0.25f, 0.03f, 2, 30, 10);
	new CLineShooter(-0.4f, -0.6f, 0.25f, 0.025f, 2, 25, 10);
	new CLineShooter(0, -0.7f, 0.25f, 0.02f, 2, 20, 10);
	new CLineShooter(0.4f, -0.6f, 0.25f, 0.025f, 2, 25, 10);
	new CLineShooter(0.8f, -0.4f, 0.25f, 0.03f, 2, 30, 10);
}

void Line2() {
	new CLineShooter(-0.8f, -0.4f, 0.1f, 0.03f, 2, 30, 10);
	new CLineShooter(-0.4f, -0.6f, 0.3f, 0.025f, 2, 25, 10);
	new CLineShooter(0, -0.7f, 0.25f, 0.02f, 2, 20, 10);
	new CLineShooter(0.4f, -0.6f, 0.2f, 0.025f, 2, 25, 10);
	new CLineShooter(0.8f, -0.4f, 0.4f, 0.03f, 2, 30, 10);
}

//==============================================================
class CNWayLineShooter : public CLineShooter {
public:
	float ShotAngleRange;
	int ShotCount;

	CNWayLineShooter(
		float x, float y, 
		float angle, float speed, 
		int interval, int shot_time, int wait_time,
		float angle_range, int count)
	:	CLineShooter(x, y, angle, speed, interval, shot_time, wait_time),
		ShotAngleRange(angle_range), ShotCount(count)
	{
		X=x;
		Y=y;
	}

	void Move() {
		if (Time<ShotTime && Time%Interval==0) {
			ShootNWay(PsNeedle, X, Y, ShotAngle, ShotAngleRange, ShotSpeed, ShotCount, 0, 0);
		}
		Time=(Time+1)%(ShotTime+WaitTime);
	}
};

void NWayLine() {
	new CNWayLineShooter(-0.8f, -0.4f, 0.1f, 0.03f, 2, 30, 10, 0.25, 3);
	new CNWayLineShooter(-0.4f, -0.6f, 0.3f, 0.025f, 2, 25, 10, 0.25, 3);
	new CNWayLineShooter(0, -0.7f, 0.25f, 0.02f, 2, 20, 10, 0.25, 3);
	new CNWayLineShooter(0.4f, -0.6f, 0.2f, 0.025f, 2, 25, 10, 0.25, 3);
	new CNWayLineShooter(0.8f, -0.4f, 0.4f, 0.03f, 2, 30, 10, 0.25, 3);
}

//==============================================================
class CAimingLineShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotSpeed;
	int Interval;
	int ShotTime;
	int WaitTime;
	int Time;

	CAimingLineShooter(
		float x, float y, float speed, 
		int interval, int shot_time, int wait_time)
	:	CEnemy(PsEnemy),
		ShotAngle(0), ShotSpeed(speed), 
		Interval(interval), ShotTime(shot_time), 
		WaitTime(wait_time), Time(0)
	{
		X=x;
		Y=y;
	}

	void Move() {
		if (Time==0) {
			ShotAngle=GetMyShipAngle(X, Y);
		}
		if (Time<ShotTime && Time%Interval==0) {
			new CBullet(PsNeedle, X, Y, ShotAngle, 0, ShotSpeed, 0);
		}
		Time=(Time+1)%(ShotTime+WaitTime);
	}
};

void AimingLine() {
	new CAimingShooter(0, -0.7f, 0.02f, 2);
}

void AimingLine2() {
	new CAimingLineShooter(0, -0.7f, 0.02f, 2, 20, 10);
}

void AimingLine3() {
	new CAimingLineShooter(-0.8f, -0.4f, 0.03f, 2, 30, 10);
	new CAimingLineShooter(-0.4f, -0.6f, 0.025f, 2, 25, 10);
	new CAimingLineShooter(0, -0.7f, 0.02f, 2, 20, 10);
	new CAimingLineShooter(0.4f, -0.6f, 0.025f, 2, 20, 10);
	new CAimingLineShooter(0.8f, -0.4f, 0.03f, 2, 30, 10);
}

void AimingLine4() {
	new CLineShooter(-0.8f, -0.4f, 0.1f, 0.03f, 2, 30, 10);
	new CAimingLineShooter(-0.4f, -0.6f, 0.025f, 2, 25, 10);
	new CLineShooter(0, -0.7f, 0.25f, 0.02f, 2, 20, 10);
	new CAimingLineShooter(0.4f, -0.6f, 0.025f, 2, 25, 10);
	new CLineShooter(0.8f, -0.4f, 0.4f, 0.03f, 2, 30, 10);
}

//==============================================================
class CSpreadingShooter : public CEnemy {
public:
	float ShotAngleRange;
	float ShotSpeed;
	int ShotCount;
	int Interval;
	float GroupSpeed;
	int GroupCount;
	int Time;

	CSpreadingShooter(
		float angle_range, float speed, int count, int interval,
		float group_speed, int group_count
	)
	:	CEnemy(IkEnemy),
		ShotAngleRange(angle_range), ShotSpeed(speed), 
		ShotCount(count), Interval(interval), 
		GroupSpeed(group_speed), GroupCount(group_count), 
		Time(0)
	{}

	void Move() {
		if (Time==0) {
			for (int i=0; i<GroupCount; i++) {
				ShootNWay(
					DpRedBullet, X, Y, 
					GetMyShipAngle(X, Y), ShotAngleRange, 
					ShotSpeed+GroupSpeed*i, ShotCount, 0, 0);
			}
		}
		Time=(Time+1)%Interval;
	}	
};

void Spreading() {
	new CSpreadingShooter(0.2f, 0.01f, 9, 240, 0.005f, 4);
}

void Spreading2() {
	CMover* mover=new CSpreadingShooter(0.2f, 0.0125f, 9, 240, 0.005f, 4);
	mover->SetShape(ShVoid);
	new CSpreadingShooter(0.175f, 0.01f, 8, 240, 0.005f, 4);
}

//==============================================================
class CRandomSpreadingShooter : public CEnemy {
public:
	float ShotAngleRange;
	float ShotSpeed;
	float ShotSpeedRange;
	int ShotCount;
	int Interval;
	int Time;

	CRandomSpreadingShooter(
		float angle_range, float speed, float speed_range, 
		int count, int interval
	)
	:	CEnemy(IkEnemy),
		ShotAngleRange(angle_range), 
		ShotSpeed(speed), ShotSpeedRange(speed_range), 
		ShotCount(count), Interval(interval), 
		Time(0)
	{}

	void Move() {
		if (Time==0) {
			for (int i=0; i<ShotCount; i++) {
				new CBullet(
					DpRedBullet, X, Y, 
					GetMyShipAngle(X, Y)+ShotAngleRange*(Rand.Real1()-0.5f), 0, 
					ShotSpeed+ShotSpeedRange*(Rand.Real1()-0.5f), 0);
			}
		}
		Time=(Time+1)%Interval;
	}	
};

void RandomSpreading() {
	new CRandomSpreadingShooter(0.2f, 0.02f, 0.02f, 30, 240);
}

void RandomSpreading2() {
	new CRandomSpreadingShooter(0.2f, 0.02f, 0.02f, 60, 240);
}

void RandomSpreading3() {
	CMover* mover;
	mover=new CRandomSpreadingShooter(0.2f, 0.02f, 0.02f, 30, 240);
	mover->X=-0.6f;
	mover=new CRandomSpreadingShooter(0.2f, 0.02f, 0.02f, 30, 240);
	mover->X=0.6f;
}

//==============================================================
class COvertakingShooter : public CEnemy {
public:
	float ShotAngleRange;
	float ShotSpeed;
	int ShotCount;
	int Interval;
	float GroupSpeed;
	float GroupAngle;
	int GroupCount;
	int GroupInterval;
	float ShotAngle;
	int Time;

	COvertakingShooter(
		float angle_range, float speed, int count, int interval, 
		float group_speed, float group_angle, 
		int group_count, int group_interval
	)
	:	CEnemy(IkEnemy),
		ShotAngleRange(angle_range), ShotSpeed(speed), 
		ShotCount(count), Interval(interval), 
		GroupSpeed(group_speed), GroupAngle(group_angle), 
		GroupCount(group_count), GroupInterval(group_interval), 
		ShotAngle(0), Time(0)
	{}

	void Move() {
		if (Time==0) {
			ShotAngle=GetMyShipAngle(X, Y);
		}
		int i=Time/GroupInterval;
		if (i<GroupCount && Time%GroupInterval==0) {
			ShootNWay(
				DpRedBullet, X, Y, 
				ShotAngle+GroupAngle*i, ShotAngleRange, 
				ShotSpeed+GroupSpeed*i, ShotCount, 0, 0);
		}
		Time=(Time+1)%Interval;
	}	
};

void Overtaking() {
	new COvertakingShooter(0.2f, 0.01f, 7, 240, 0.002f, 0, 10, 4);
}

void Overtaking2() {
	new COvertakingShooter(0.2f, 0.01f, 7, 240, 0.002f, 0.003f, 10, 4);
}

void Overtaking3() {
	new COvertakingShooter(0.2f, 0.03f, 7, 240, -0.002f, 0, 10, 4);
}

//==============================================================
class CColoredSpiralShooter : public CBiDirectionalSpiralShooter {
public:

	CColoredSpiralShooter(float angle, float angle_rate0, float angle_rate1, float speed, int count, int interval) 
	:	CBiDirectionalSpiralShooter(angle, angle_rate0, angle_rate1, speed, count, interval)
	{
		SetShape(IkEnemy);
	}

	void Move() {
		if (Time==0) {
			for (int j=0; j<2; j++) {
				for (int i=0; i<ShotCount; i++) {
					CMover* mover=new CBullet(j==0?ThBlackBullet:ThWhiteBullet, X, Y, ShotAngle[j]+(float)i/ShotCount, 0, ShotSpeed, 0);
					mover->Color=j;
				}
				ShotAngle[j]+=ShotAngleRate[j];
				ShotAngle[j]-=floor(ShotAngle[j]);
			}
		}
		Time=(Time+1)%Interval;
	}
};

void ColoredSpiral() {
	MyShip->Color=0;
	new CColoredSpiralShooter(0, -0.003f, 0.002f, 0.02f, 8, 3);
}

//==============================================================
class CColoredRandomNWayShooter : public CRandomNWayShooter {
public:

	CColoredRandomNWayShooter(float x, float y, float angle_range, float speed, int interval, int color)
	:	CRandomNWayShooter(0, angle_range, speed, 1, interval)
	{
		SetShape(IkEnemy);
		X=x;
		Y=y;
		Color=color;
	}

	void Move() {
		if (Time==0) {
			CMover* mover=new CBullet(
				Color==0?IkBlackBullet:IkWhiteBullet, X, Y, 
				GetMyShipAngle(X, Y)+ShotAngleRange*(Rand.Real1()-0.5f), 
				0, ShotSpeed, 0);
			mover->Color=Color;
		}
		Time=(Time+1)%Interval;
	}
};

void ColoredRandomNWay() {
	MyShip->Color=0;
	new CColoredRandomNWayShooter(-0.7f, -0.6f, 0.2f, 0.02f, 2, 0);
	new CColoredRandomNWayShooter(0.7f, -0.6f, 0.2f, 0.02f, 2, 1);
}

//==============================================================
class CColoredCombinationShooter : public CEnemy {
public:
	CColoredSpiralShooter* Spiral;
	CColoredRandomNWayShooter* NWay;
	int Time;
	
	CColoredCombinationShooter()
	:	CEnemy(IkEnemy), Spiral(NULL), NWay(NULL), Time(0)
	{}
	
	void Move() {
		switch (Time) {
			case 0:
				if (Spiral) Spiral->Alive=false;
				NWay=new CColoredRandomNWayShooter(X, Y, 1, 0.02f, 1, 0);
				NWay->SetShape(ShVoid);
				break;
			case 100:
				Spiral=new CColoredSpiralShooter(0, -0.003f, 0.002f, 0.02f, 8, 3);
				Spiral->SetShape(ShVoid);
				break;
			case 200:
				NWay->Alive=false;
				break;
			case 300:
				Spiral->Alive=false;
				NWay=new CColoredRandomNWayShooter(X, Y, 1, 0.02f, 1, 1);
				NWay->SetShape(ShVoid);
				break;
			case 400:
				Spiral=new CColoredSpiralShooter(0, 0.002f, -0.003f, 0.02f, 8, 3);
				Spiral->SetShape(ShVoid);
				break;
			case 500:
				NWay->Alive=false;
				break;
		}
		Time=(Time+1)%600;
	}
};

void ColoredCombination() {
	MyShip->Color=0;
	new CColoredCombinationShooter();
}

