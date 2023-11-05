//==============================================================
class CSimpleHomingBullet : public CBullet {
public:
	CSimpleHomingBullet(int shape_id, float x, float y, float angle, float speed)
	:	CBullet(shape_id, x, y, angle, 0, speed, 0)
	{}
	
	void Move() {
		Angle=GetMyShipAngle(X, Y);
		CBullet::Move();
	}
};

class CSimpleHomingShooter : public CEnemy {
public:
	float ShotSpeed;
	int Interval;
	int Time;

	CSimpleHomingShooter(float x, float y, float speed, int interval)
	:	CEnemy(ThEnemy),
		ShotSpeed(speed), Interval(interval), Time(0)
	{
		X=x;
		Y=y;
	}

	void Move() {
		if (Time==0) {
			new CSimpleHomingBullet(ThWhiteBullet, X, Y, GetMyShipAngle(X, Y), ShotSpeed);
		}
		Time=(Time+1)%Interval;
	}
};

void SimpleHoming() {
	new CSimpleHomingShooter(0, -0.4f, 0.02f, 20);
}

//==============================================================
class CConstrainedHomingBullet : public CBullet {
public:
	float MaxAngleRate;

	CConstrainedHomingBullet(int shape_id, float x, float y, float angle, float speed, float max_angle_rate)
	:	CBullet(shape_id, x, y, angle, 0, speed, 0),
		MaxAngleRate(max_angle_rate)
	{}
	
	void Move() {
		float angle_rate=GetMyShipAngle(X, Y)-Angle;
		angle_rate-=floor(angle_rate);
		if (angle_rate<=MaxAngleRate || 1-angle_rate<=MaxAngleRate) {
			Angle+=angle_rate;
		} else {
			Angle+=(angle_rate<0.5f)?MaxAngleRate:-MaxAngleRate;
		}
		Angle-=floor(Angle);

		CBullet::Move();
	}
};

class CConstrainedHomingShooter : public CSimpleHomingShooter {
public:
	float MaxAngleRate;

	CConstrainedHomingShooter(float x, float y, float speed, int interval, float max_angle_rate)
	:	CSimpleHomingShooter(x, y, speed, interval),
		MaxAngleRate(max_angle_rate)
	{}

	void Move() {
		if (Time==0) {
			new CConstrainedHomingBullet(ThBlackBullet, X, Y, GetMyShipAngle(X, Y), ShotSpeed, MaxAngleRate);
		}
		Time=(Time+1)%Interval;
	}
};

void ConstrainedHoming() {
	new CConstrainedHomingShooter(0, -0.4f, 0.02f, 20, 0.005f);
}

void ConstrainedHoming2() {
	new CConstrainedHomingShooter(0, -0.4f, 0.01f, 20, 0.005f);
}

void ConstrainedHoming3() {
	new CConstrainedHomingShooter(-0.6f, 0, 0.015f, 40, 0.005f);
	new CConstrainedHomingShooter(-0.3f, -0.4f, 0.015f, 40, 0.005f);
	new CConstrainedHomingShooter(0.3f, -0.4f, 0.015f, 40, 0.005f);
	new CConstrainedHomingShooter(0.6f, 0, 0.015f, 40, 0.005f);
}

//==============================================================
class CBulletTrailer : public CEnemy {
public:
	float Speed;
	float MaxAngleRate;
	int Interval;
	int Time;

	CBulletTrailer(float x, float y, float speed, float max_angle_rate, int interval, int color)
	:	CEnemy(ThEnemy),
		Speed(speed), MaxAngleRate(max_angle_rate), 
		Interval(interval), Time(0)
	{
		X=x;
		Y=y;
		Color=color;
	}
	
	void Move() {
		if (Time==0) {
			CMover* mover=new CBullet(Color==0?ThBlackBullet:ThWhiteBullet, X, Y, Angle, 0, 0, 0);
			mover->Color=Color;
		}
		Time=(Time+1)%Interval;
		
		float angle_rate=GetMyShipAngle(X, Y)-Angle;
		angle_rate-=floor(angle_rate);
		if (angle_rate<=MaxAngleRate || 1-angle_rate<=MaxAngleRate) {
			Angle+=angle_rate;
		} else {
			Angle+=(angle_rate<0.5f)?MaxAngleRate:-MaxAngleRate;
		}
		Angle-=floor(Angle);
		
		float rad=Angle*D3DX_PI*2;
		X+=Speed*cosf(rad);
		Y+=Speed*sinf(rad);
	}
};

void BulletTrailer() {
	new CBulletTrailer(0, -0.4f, 0.01f, 0.005f, 5, 0);
}

void BulletTrailer2() {
	MyShip->Color=0;
	new CBulletTrailer(-0.4f, -0.4f, 0.01f, 0.005f, 5, 0);
	new CBulletTrailer(0.4f, -0.4f, 0.01f, 0.005f, 5, 1);
}

//==============================================================
void EvenOvertaking() {
	new COvertakingShooter(0.2f, 0.01f, 8, 240, 0.002f, 0, 10, 4);
}

void MakeEvenOvertakingShooter(int count) {
	for (int i=0; i<count; i++) {
		CMover* mover=new COvertakingShooter(0.2f, 0.01f, 8, 240, 0.002f, 0, 10, 4);
		mover->X=(i+1)*2.0f/(count+1)-1;
	}
}

void EvenOvertaking2() {
	MakeEvenOvertakingShooter(2);
}

void EvenOvertaking3() {
	MakeEvenOvertakingShooter(4);
}

void EvenOvertaking4() {
	MakeEvenOvertakingShooter(8);
}

//==============================================================
void Gap() {
	new CNWayShooter(0.75f, 0.95f, 0.005f, 200, 150, 0, 0);
}

class CGapShooter : public CEnemy {
public:
	CNWayShooter* NWay;

	CGapShooter(float angle_range, float speed, int count, int interval)
	:	CEnemy(ShVoid)
	{
		NWay=new CNWayShooter(0, angle_range, speed, count, interval, 0, 0);
		NWay->X=0;
		NWay->Y=-0.2f;
	}
	
	void Move() {
		NWay->ShotAngle=Rand.Real2();
	}
};

void Gap2() {
	new CGapShooter(0.95f, 0.005f, 200, 150);
}

void Gap3() {
	new CGapShooter(0.95f, 0.003f, 200, 100);
}

//==============================================================
class CPatternShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRange;
	float ShotSpeed;
	int Interval;
	char* Pattern;
	int Width;
	int Height;
	int Time;

	CPatternShooter(float angle, float angle_range, float speed, int interval, char* pattern, int width, int height)
	:	CEnemy(PsEnemy),
		ShotAngle(angle), 
		ShotAngleRange(angle_range), 
		ShotSpeed(speed),
		Interval(interval), 
		Pattern(pattern), Width(width), Height(height), 
		Time(0)
	{}

	void Move() {
		if (Time%Interval==0) {
			char* p=Pattern+(Height-1-Time/Interval)*Width;
			for (int i=Width-1; i>=0; i--, p++) {
				if (*p!=' ') {
					new CBullet(
						IkBlackBullet, X, Y, 
						ShotAngle+ShotAngleRange*((float)i/(Width-1)-0.5f), 
						0, ShotSpeed, 0);
				}
			}
		}
		Time=(Time+1)%(Interval*Height);
	}
};

void Pattern() {
	static char pattern[]=
		"                                   "
		"                                   "
		"                                   "
		"####  #   # #     #     ##### #####"
		"#   # #   # #     #     #       #  "
		"####  #   # #     #     ####    #  "
		"#   # #   # #     #     #       #  "
		"####  ##### ##### ##### #####   #  ";
	new CPatternShooter(0.25f, 0.3f, 0.01f, 5, pattern, 35, 8);
}

void Pattern2() {
	static char pattern[]=
		"#########   ##################"
		"#########   ##################"
		"#########   ##################"
		"######      ###            ###"
		"######      ###            ###"
		"######      ###            ###"
		"######   ######   ######   ###"
		"######   ######   ######   ###"
		"######   ######   ######   ###"
		"######            ###      ###"
		"######            ###      ###"
		"######            ###      ###"
		"#####################   ######"
		"#####################   ######"
		"#####################   ######"
		"#########               ######"
		"#########               ######"
		"#########               ######"
		"#########   ##################"
		"#########   ##################"
		"#########   ##################";
	new CPatternShooter(0.25f, 0.3f, 0.005f, 10, pattern, 30, 21);
}

//==============================================================
class CSteppingBullet : public CBullet {
public:
	float InitialSpeed;
	int MoveTime;
	int StopTime;
	int Time;

	CSteppingBullet(float x, float y, float angle, float speed, int move_time, int stop_time)
	:	CBullet(ThBlackBullet, x, y, angle, 0, speed, 0),
		InitialSpeed(speed),
		MoveTime(move_time), StopTime(stop_time), Time(0)
	{}
	
	void Move() {
		if (Time==0) {
			Speed=InitialSpeed;
			SetShape(ThBlackBullet);
		} else
		if (Time==MoveTime) {
			Speed=0;
			SetShape(ThWhiteBullet);
		}
		Time=(Time+1)%(MoveTime+StopTime);
		CBullet::Move();
	}
};

class CSteppingSpiralShooter : public CEnemy {
public:
	float ShotAngle;
	float ShotAngleRate;
	float ShotSpeed;
	int MoveTime;
	int StopTime;

	CSteppingSpiralShooter(float angle, float angle_rate, float speed, int move_time, int stop_time) 
	:	CEnemy(ThEnemy),
		ShotAngle(angle), 
		ShotAngleRate(angle_rate), 
		ShotSpeed(speed),
		MoveTime(move_time), StopTime(stop_time)
		{}

	void Move() {
		new CSteppingBullet(X, Y, ShotAngle, ShotSpeed, MoveTime, StopTime);
		ShotAngle+=ShotAngleRate;
		ShotAngle-=floor(ShotAngle);
	}
};

void Stepping() {
	new CSteppingSpiralShooter(0, 0.03f, 0.01f, 60, 20);
}

void Stepping2() {
	new CSteppingSpiralShooter(0, 0.03f, 0.01f, 60, 60);
}

//==============================================================
class CAimingAgainBullet : public CBullet {
public:
	float InitialSpeed;
	int MoveTime;
	int StopTime;
	int Time;

	CAimingAgainBullet(float x, float y, float speed, int move_time, int stop_time)
	:	CBullet(ThBlackBullet, x, y, GetMyShipAngle(x, y), 0, speed, 0),
		InitialSpeed(speed),
		MoveTime(move_time), StopTime(stop_time), Time(0)
	{}
	
	void Move() {
		if (Time==0) {
			Angle=GetMyShipAngle(X, Y);
			Speed=InitialSpeed;
			SetShape(ThBlackBullet);
		} else
		if (Time==MoveTime) {
			Speed=0;
			SetShape(ThWhiteBullet);
		}
		Time=(Time+1)%(MoveTime+StopTime);
		CBullet::Move();
	}
};

class CAimingAgainShooter : public CEnemy {
public:
	float ShotSpeed;
	int Interval;
	int MoveTime;
	int StopTime;
	int Time;

	CAimingAgainShooter(float x, float y, float speed, int interval, int move_time, int stop_time)
	:	CEnemy(ThEnemy),
		ShotSpeed(speed),
		Interval(interval), 
		MoveTime(move_time), StopTime(stop_time),
		Time(0)
	{
		X=x;
		Y=y;
	}

	void Move() {
		if (Time==0) {
			new CAimingAgainBullet(X, Y, ShotSpeed, MoveTime, StopTime);
		}
		Time=(Time+1)%Interval;
	}
};

void AimingAgain() {
	new CAimingAgainShooter(0, -0.7f, 0.02f, 40, 60, 20);
}

void AimingAgain2() {
	new CAimingAgainShooter(0, -0.7f, 0.02f, 40, 20, 20);
}

void AimingAgain3() {
	new CAimingAgainShooter(-0.6f, -0.6f, 0.01f, 40, 60, 20);
	new CAimingAgainShooter(-0.3f, -0.7f, 0.015f, 42, 60, 20);
	new CAimingAgainShooter(0, -0.8f, 0.02f, 44, 60, 20);
	new CAimingAgainShooter(0.3f, -0.7f, 0.015f, 42, 60, 20);
	new CAimingAgainShooter(0.6f, -0.6f, 0.01f, 40, 60, 20);
}

//==============================================================
typedef void (*CREATE_BULLET)(float x, float y);

class CAnyShooter : public CEnemy {
public:
	CREATE_BULLET Func;
	int Interval;
	int Time;

	CAnyShooter(int shape_id, float x, float y, CREATE_BULLET func, int interval)
	:	CEnemy(shape_id), Func(func),
		Interval(interval), Time(0)
	{
		X=x;
		Y=y;
	}

	void Move() {
		if (Time==0) {
			Func(X, Y);
		}
		Time=(Time+1)%Interval;
	}
};

void CreateAimingAgainBullet(float x, float y) {
	new CAimingAgainBullet(x, y, 0.02f, 60, 20);
}

void Any() {
	new CAnyShooter(ThEnemy, 0, -0.7f, CreateAimingAgainBullet, 60);
}

void CreateConstrainedHomingBullet(float x, float y) {
	new CConstrainedHomingBullet(ThBlackBullet, x, y, GetMyShipAngle(x, y), 0.02f, 0.005f);
}

void Any2() {
	new CAnyShooter(ThEnemy, 0, -0.4f, CreateConstrainedHomingBullet, 20);
}

//==============================================================
class CSplittingBullet : public CBullet {
public:
	int SplitTime;
	int SplitCount;
	int SplitInterval;
	int Time;
	float ShotAngle;

	CSplittingBullet(float x, float y, float angle, float speed, int split_time, int split_count, int split_interval)
	:	CBullet(IkBlackBullet, x, y, angle, 0, speed, 0),
		SplitTime(split_time), SplitCount(split_count), SplitInterval(split_interval), Time(0)
	{}
	
	void Move() {
		if (Time==SplitTime) {
			SetShape(ShVoid);
			ShotAngle=GetMyShipAngle(X, Y);
		}
		if (Time<SplitTime) {
			CBullet::Move();
		} else
		if (Time<SplitTime+SplitCount*SplitInterval) {
			if ((Time-SplitTime)%SplitInterval==0) {
				new CBullet(ThBlackBullet, X, Y, ShotAngle, 0, Speed, 0);
			}
		} else {
			Alive=false;
		}
		Time++;
	}
};

void CreateSplittingBullet(float x, float y) {
	new CSplittingBullet(x, y, GetMyShipAngle(x, y), 0.01f, 30, 5, 4);
}

void Splitting() {
	new CAnyShooter(ThEnemy, 0, -0.7f, CreateSplittingBullet, 60);
}

//==============================================================
typedef void (*CREATE_NWAY_BULLET)(float x, float y, float angle);

void ShootAnyNWay(CREATE_NWAY_BULLET func, float x, float y, float angle, float angle_range, int count) {
	if (count>1) {
		for (int i=0; i<count; i++) {
			func(x, y, angle+angle_range*((float)i/(count-1)-0.5f)); 
		}
	} else
	if (count==1) {
		func(x, y, angle);
	}
}

class CAnyNWayShooter : public CEnemy {
public:
	CREATE_NWAY_BULLET Func;
	float ShotAngle;
	float ShotAngleRange;
	float ShotCount;
	int Interval;
	int Time;

	CAnyNWayShooter(int shape_id, float x, float y, CREATE_NWAY_BULLET func, float angle, float angle_range, int count, int interval)
	:	CEnemy(shape_id), Func(func), 
		ShotAngle(angle), ShotAngleRange(angle_range), 
		ShotCount(count), Interval(interval), Time(0)
	{
		X=x;
		Y=y;
	}

	void Move() {
		if (Time==0) {
			ShootAnyNWay(Func, X, Y, ShotAngle, ShotAngleRange, ShotCount);
		}
		Time=(Time+1)%Interval;
	}
};

void CreateSplittingNWayBullet(float x, float y, float angle) {
	new CSplittingBullet(x, y, angle, 0.01f, 30, 5, 4);
}

void SplittingNWay() {
	new CAnyNWayShooter(ThEnemy, 0, -0.7f, CreateSplittingNWayBullet, 0.25f, 0.4f, 7, 60);
}

//==============================================================
class CPlacedBullet : public CBullet {
public:
	float InitialSpeed;
	int MoveTime;
	int StopTime;
	int Time;
	
	CPlacedBullet(int shape_id, float x, float y, float angle, float speed, int move_time, int stop_time) 
	:	CBullet(shape_id, x, y, angle, 0, speed, 0),
		InitialSpeed(speed), 
		MoveTime(move_time), StopTime(stop_time), Time(0)
	{}
	
	void Move() {
		if (Time==MoveTime) {
			Speed=0;
		}
		if (Time==MoveTime+StopTime) {
			Speed=InitialSpeed;
		}
		Time++;
		CBullet::Move();
	}
};

void CreatePlacedBullet(float x, float y) {
	new CPlacedBullet(ThBlackBullet, x, y, GetMyShipAngle(x, y), 0.01f, 60, 120);
}

void Placed() {
	new CAnyShooter(IkEnemy, 0, -0.7f, CreatePlacedBullet, 20);
}

//==============================================================
class CSpiralPlacedShooter : public CEnemy {
public:
	float OrbitAngle;
	float OrbitAngleRate;
	float OrbitRadius;
	int ShotTime;
	int WaitTime;
	int Interval;
	int Cycle;
	int Time;
	int BulletShapeId;
	float BulletSpeed;
	int GroupCount;
	
	CSpiralPlacedShooter(
		float orbit_angle, float orbit_angle_rate, float orbit_radius, 
		int shot_time, int wait_time, int interval, int cycle, 
		int bullet_shape_id, float bullet_speed, int group_count)
	:	CEnemy(IkEnemy),
		OrbitAngle(orbit_angle), OrbitAngleRate(orbit_angle_rate), OrbitRadius(orbit_radius), 
		ShotTime(shot_time), WaitTime(wait_time), Interval(interval), Cycle(cycle), Time(0), 
		BulletShapeId(bullet_shape_id), BulletSpeed(bullet_speed), 
		GroupCount(group_count)
	{
		float rad=OrbitAngle*D3DX_PI*2;
		X=cosf(rad)*OrbitRadius;
		Y=sinf(rad)*OrbitRadius;
	}
	
	void Move() {
		float rad=OrbitAngle*D3DX_PI*2;
		X=cosf(rad)*OrbitRadius;
		Y=sinf(rad)*OrbitRadius;

		OrbitAngle+=OrbitAngleRate;
		OrbitAngle-=floor(OrbitAngle);

		new CBullet(BulletShapeId, X, Y, OrbitAngle, 0, BulletSpeed, 0);

		int count=Time/(ShotTime+WaitTime);
		int time=Time%(ShotTime+WaitTime);
		float base_time=(OrbitRadius-MyShip->Shape->Size)/BulletSpeed;
		if (count<GroupCount) {
			if (time<ShotTime && time%Interval==0) {
				new CPlacedBullet(
					BulletShapeId, X, Y, OrbitAngle+0.5f, BulletSpeed, 
					(int)(base_time*count/GroupCount),
					(int)(base_time+(ShotTime+WaitTime)*(GroupCount-count)));			
			}
		}
		Time=(Time+1)%Cycle;
	}
};

void SpiralPlaced() {
	new CSpiralPlacedShooter(0, 0.003f, 0.9f, 20, 20, 3, 700, ThBlackBullet, 0.01f, 10);
	new CSpiralPlacedShooter(0.5f, 0.003f, 0.9f, 20, 20, 3, 700, ThWhiteBullet, 0.01f, 10);
}

void SpiralPlaced2() {
	new CSpiralPlacedShooter(0, 0.004f, 0.9f, 35, 5, 3, 600, ThBlackBullet, 0.02f, 10);
	new CSpiralPlacedShooter(0.5f, 0.004f, 0.9f, 35, 5, 3, 600, ThWhiteBullet, 0.02f, 10);
}

void SpiralPlaced3() {
	new CSpiralPlacedShooter(0.1f, 0.004f, 0.9f, 20, 20, 3, 600, ThBlackBullet, 0.018f, 10);
	new CSpiralPlacedShooter(0.6f, 0.004f, 0.9f, 20, 20, 3, 600, ThBlackBullet, 0.018f, 10);
	new CSpiralPlacedShooter(0.35f, -0.003f, 0.8f, 25, 15, 3, 600, ThWhiteBullet, 0.016f, 10);
	new CSpiralPlacedShooter(0.85f, -0.003f, 0.8f, 25, 15, 3, 600, ThWhiteBullet, 0.016f, 10);
}

//==============================================================
class CStarPlacedShooter;
typedef void (*CREATE_STAR_PLACED_BULLET)(CStarPlacedShooter* s, float x, float y);

class CStarPlacedShooter : public CEnemy {
public:
	float StarAngle;
	float StarSize;
	int ShotTime;
	int WaitTime;
	int Interval;
	int Cycle;
	int Time;
	int BulletShapeId;
	float BulletSpeed;
	float BulletAngle;
	CREATE_STAR_PLACED_BULLET Func;
	
	CStarPlacedShooter(
		float x, float y, float star_angle, float star_size, 
		int shot_time, int wait_time, int interval, int cycle, 
		int bullet_shape_id, float bullet_speed, float bullet_angle, 
		CREATE_STAR_PLACED_BULLET func)
	:	CEnemy(ThEnemy), 
		StarAngle(star_angle), StarSize(star_size), 
		ShotTime(shot_time), WaitTime(wait_time), 
		Interval(interval), Cycle(cycle), Time(0),
		BulletShapeId(bullet_shape_id), BulletSpeed(bullet_speed),
		BulletAngle(bullet_angle), Func(func)
	{
		X=x;
		Y=y;
	}

	void Move() {
		static const float 
			x[]={0, 0.59f, -0.95f, 0.95f, -0.59f, 0},
			y[]={-1, 0.81f, -0.31f, -0.31f, 0.81f, -1};
		int line_time=ShotTime/5;
		if (Time<ShotTime && Time%Interval==0) {
			int line=Time/line_time;
			int time=Time%line_time;
			float
				ax=(x[line]+(x[line+1]-x[line])*time/line_time),
				ay=(y[line]+(y[line+1]-y[line])*time/line_time);
			float
				rad=StarAngle*D3DX_PI*2, 
				c=cosf(rad), s=sinf(rad), 
				bx=X+(ax*c-ay*s)*StarSize,
				by=Y+(ax*s+ay*c)*StarSize;
			Func(this, bx, by);
		}
		Time=(Time+1)%Cycle;
	}
};

void CreateStarPlacedBullet(CStarPlacedShooter* s, float x, float y) {
	new CPlacedBullet(
		s->BulletShapeId, x, y,  
		GetAngle(s->X, s->Y, x, y)+s->BulletAngle,
		s->BulletSpeed, 0, 
		s->ShotTime+s->WaitTime-s->Time);
}

void StarPlaced() {
	new CStarPlacedShooter(0, -0.4f, 0, 0.3f, 100, 30, 2, 250, IkBlackBullet, 0.01f, 0.4f, CreateStarPlacedBullet);
}

void StarPlaced2() {
	CMover* mover=new CStarPlacedShooter(0, -0.4f, 0, 0.3f, 100, 30, 2, 250, IkBlackBullet, 0.009f, 0.4f, CreateStarPlacedBullet);
	mover->SetShape(ShVoid);
	new CStarPlacedShooter(0, -0.4f, 0.5f, 0.5f, 100, 30, 2, 250, IkWhiteBullet, 0.015f, -0.4f, CreateStarPlacedBullet);
}

//==============================================================
class CPlacedTwiceBullet : public CBullet {
public:
	float PlacedAngle[2];
	float PlacedSpeed[2];
	int PlacedTime[3];
	int Time;
	
	CPlacedTwiceBullet(
		int shape_id, float x, float y, float angle, 
		float angle0, float angle1, 
		float speed0, float speed1,
		int time0, int time1, int time2)
	:	CBullet(shape_id, x, y, angle, 0, 0, 0), 
		Time(0)
	{
		PlacedAngle[0]=angle0;
		PlacedAngle[1]=angle1;
		
		PlacedSpeed[0]=speed0;
		PlacedSpeed[1]=speed1;

		PlacedTime[0]=time0;
		PlacedTime[1]=time1;
		PlacedTime[2]=time2;
	}
	
	void Move() {
		if (Time==PlacedTime[0]) {
			Angle=PlacedAngle[0];
			Speed=PlacedSpeed[0];
		}
		if (Time==PlacedTime[0]+PlacedTime[1]) {
			Speed=0;
		}
		if (Time==PlacedTime[0]+PlacedTime[1]+PlacedTime[2]) {
			Angle=PlacedAngle[1];
			Speed=PlacedSpeed[1];
		}
		Time++;
		CBullet::Move();
	}
};

void CreateStarPlacedTwiceBullet(CStarPlacedShooter* s, float x, float y) {
	for (int i=0; i<5; i++) {
		new CPlacedTwiceBullet(
			s->BulletShapeId, x, y, 0, 
			s->StarAngle+(float)i/5, 
			GetAngle(s->X, s->Y, x, y)+s->BulletAngle,
			s->BulletSpeed, s->BulletSpeed, 
			s->ShotTime+s->WaitTime-s->Time, 
			s->StarSize*1.5f/s->BulletSpeed, s->WaitTime);
	}
}

void StarPlacedTwice() {
	new CStarPlacedShooter(0, -0.4f, 0, 0.3f, 100, 30, 2, 400, IkBlackBullet, 0.01f, 0.4f, CreateStarPlacedTwiceBullet);
}

void StarPlacedTwice2() {
	CMover* mover=new CStarPlacedShooter(0, -0.4f, 0, 0.3f, 100, 30, 2, 400, IkBlackBullet, 0.009f, 0.4f, CreateStarPlacedTwiceBullet);
	mover->SetShape(ShVoid);
	new CStarPlacedShooter(0, -0.4f, 0.5f, 0.5f, 100, 30, 2, 400, IkWhiteBullet, 0.015f, -0.4f, CreateStarPlacedTwiceBullet);
}



