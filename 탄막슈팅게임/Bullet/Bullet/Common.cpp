#include "Common.h"
#include "Mover.h"
#include "Stage3.h"
#include "Stage4.h"
#include "Stage5.h"
#include "Stage6.h"

//==============================================================
// シェイプ
SHAPE ShapeList[]={
	{L"ShVoid", 0, 0},
	{L"ShHit", 0, 0},
	{L"DpMyShip", 0.08f, 0.01f},
	{L"DpEnemy", 0.2f, 0.01f},
	{L"DpRedBullet", 0.04f, 0.01f},
	{L"DpBlueBullet", 0.05f, 0.01f},
	{L"PsMyShip", 0.08f, 0.01f},
	{L"PsEnemy", 0.08f, 0.01f},
	{L"PsNeedle", 0.04f, 0.01f},
	{L"PsBullet", 0.03f, 0.01f},
	{L"IkBlackMyShip", 0.08f, 0.01f},
	{L"IkWhiteMyShip", 0.08f, 0.01f},
	{L"IkEnemy", 0.08f, 0.01f},
	{L"IkBlackBullet", 0.03f, 0.01f},
	{L"IkWhiteBullet", 0.03f, 0.01f},
	{L"ThMyShip", 0.08f, 0.01f},
	{L"ThEnemy", 0.08f, 0.01f},
	{L"ThBlackBullet", 0.04f, 0.01f},
	{L"ThWhiteBullet", 0.04f, 0.01f},
};
size_t ShapeCount=sizeof(ShapeList)/sizeof(SHAPE);

//==============================================================
// ステージ
STAGE Stage[]={
	{L"DIRECTIONAL", Directional, DpMyShip},
	{L"DIRECTIONAL 2", Directional2, DpMyShip},
	{L"DIRECTIONAL 3", Directional3, DpMyShip},
	{L"SPIRAL", Spiral, DpMyShip},
	{L"SPIRAL 2", Spiral2, DpMyShip},
	{L"SPIRAL 3", Spiral3, DpMyShip},
	{L"SPIRAL 4", Spiral4, DpMyShip},
	{L"SPIRAL 5", Spiral5, DpMyShip},
	{L"MULTIPLE SPIRAL", MultipleSpiral, DpMyShip},
	{L"INTERVAL MULTIPLE SPIRAL", IntervalMultipleSpiral, DpMyShip},
	{L"INTERVAL MULTIPLE SPIRAL 2", IntervalMultipleSpiral2, DpMyShip},
	{L"BI-DIRECTIONAL SPIRAL", BiDirectionalSpiral, DpMyShip},
	{L"BI-DIRECTIONAL SPIRAL 2", BiDirectionalSpiral2, DpMyShip},
	{L"BI-DIRECTIONAL SPIRAL 3", BiDirectionalSpiral3, DpMyShip},
	{L"BI-DIRECTIONAL SPIRAL 4", BiDirectionalSpiral4, DpMyShip},
	{L"BENT SPIRAL", BentSpiral, DpMyShip},
	{L"BENT SPIRAL 2", BentSpiral2, DpMyShip},
	{L"BENT SPIRAL 3", BentSpiral3, DpMyShip},
	{L"BENT SPIRAL 4", BentSpiral4, DpMyShip},
	{L"COMBINED SPIRAL", CombinedSpiral, DpMyShip},
	{L"WASHER SPIRAL", WasherSpiral, DpMyShip},
	{L"EASY WASHER SPIRAL", EasyWasherSpiral, DpMyShip},
	{L"EASY WASHER SPIRAL 2", EasyWasherSpiral2, DpMyShip},
	{L"EASY WASHER SPIRAL 3", EasyWasherSpiral3, DpMyShip},
	{L"EASY WASHER SPIRAL 4", EasyWasherSpiral4, DpMyShip},
	{L"HARD WASHER SPIRAL", HardWasherSpiral, DpMyShip},
	{L"RANKED WASHER SPIRAL", RankedWasherSpiral, DpMyShip},
	{L"RANKED WASHER SPIRAL 2", RankedWasherSpiral2, DpMyShip},
	{L"RANKED WASHER SPIRAL 3", RankedWasherSpiral3, DpMyShip},
	{L"RANKED WASHER SPIRAL 4", RankedWasherSpiral4, DpMyShip},

	{L"N-WAY", NWay, PsMyShip},
	{L"N-WAY 2", NWay2, PsMyShip},
	{L"N-WAY 3", NWay3, PsMyShip},
	{L"CIRCLE", Circle, PsMyShip},
	{L"CIRCLE 2", Circle2, PsMyShip},
	{L"CIRCLE 3", Circle3, PsMyShip},
	{L"BENT CIRCLE", BentCircle, PsMyShip},
	{L"BENT CIRCLE 2", BentCircle2, PsMyShip},
	{L"BENT CIRCLE 3", BentCircle3, PsMyShip},
	{L"DENSE N-WAY", DenseNWay, PsMyShip},
	{L"DENSE N-WAY 2", DenseNWay2, PsMyShip},
	{L"DENSE N-WAY 3", DenseNWay3, PsMyShip},
	{L"DENSE N-WAY 4", DenseNWay4, PsMyShip},
	{L"DENSE N-WAY 5", DenseNWay5, PsMyShip},
	{L"AIMING N-WAY", AimingNWay, PsMyShip},
	{L"AIMING N-WAY 2", AimingNWay2, PsMyShip},
	{L"AIMING N-WAY 3", AimingNWay3, PsMyShip},
	{L"INTERMITTENT AIMING N-WAY", IntermittentAimingNWay, PsMyShip},
	{L"RANDOM N-WAY", RandomNWay, PsMyShip},
	{L"RANDOM N-WAY 2", RandomNWay2, PsMyShip},
	{L"RANDOM N-WAY 3", RandomNWay3, PsMyShip},
	{L"RANDOM N-WAY 4", RandomNWay4, PsMyShip},
	{L"RANDOM CIRCLE", RandomCircle, PsMyShip},
	{L"ROLLING N-WAY", RollingNWay, PsMyShip},
	{L"ROLLING N-WAY2", RollingNWay2, PsMyShip},
	{L"ROLLING N-WAY3", RollingNWay3, PsMyShip},
	{L"ROLLING N-WAY4", RollingNWay4, PsMyShip},
	{L"WAVING N-WAY", WavingNWay, PsMyShip},
	{L"WAVING N-WAY 2", WavingNWay2, PsMyShip},
	{L"WAVING CIRCLE", WavingCircle, PsMyShip},

	{L"AIMING", Aiming, IkBlackMyShip},
	{L"AIMING 2", Aiming2, IkBlackMyShip},
	{L"AIMING 3", Aiming3, IkBlackMyShip},
	{L"AIMING 4", Aiming4, IkBlackMyShip},
	{L"CIRCLE + AIMING", CircleAndAiming, IkBlackMyShip},
	{L"CIRCLE + AIMING 2", CircleAndAiming2, IkBlackMyShip},
	{L"CIRCLE + AIMING 3", CircleAndAiming3, IkBlackMyShip},
	{L"AREA", Area, IkBlackMyShip},
	{L"AREA 2", Area2, IkBlackMyShip},
	{L"WINDER", Winder, IkBlackMyShip},
	{L"WINDER 2", Winder2, IkBlackMyShip},
	{L"LINE", Line, IkBlackMyShip},
	{L"LINE 2", Line2, IkBlackMyShip},
	{L"N-WAY LINE", NWayLine, IkBlackMyShip},
	{L"AIMING LINE", AimingLine, IkBlackMyShip},
	{L"AIMING LINE 2", AimingLine2, IkBlackMyShip},
	{L"AIMING LINE 3", AimingLine3, IkBlackMyShip},
	{L"AIMING LINE 4", AimingLine4, IkBlackMyShip},
	{L"SPREADING", Spreading, IkBlackMyShip},
	{L"SPREADING 2", Spreading2, IkBlackMyShip},
	{L"RANDOM SPREADING", RandomSpreading, IkBlackMyShip},
	{L"RANDOM SPREADING 2", RandomSpreading2, IkBlackMyShip},
	{L"RANDOM SPREADING 3", RandomSpreading3, IkBlackMyShip},
	{L"OVERTAKING", Overtaking, IkBlackMyShip},
	{L"OVERTAKING 2", Overtaking2, IkBlackMyShip},
	{L"OVERTAKING 3", Overtaking3, IkBlackMyShip},
	{L"COLORED SPIRAL", ColoredSpiral, IkBlackMyShip},
	{L"COLORED RANDOM N-WAY", ColoredRandomNWay, IkBlackMyShip},
	{L"COLORED COMBINATION", ColoredCombination, IkBlackMyShip},

	{L"SIMPLE HOMING", SimpleHoming, ThMyShip},
	{L"CONSTRAINED HOMING", ConstrainedHoming, ThMyShip},
	{L"CONSTRAINED HOMING 2", ConstrainedHoming2, ThMyShip},
	{L"CONSTRAINED HOMING 3", ConstrainedHoming3, ThMyShip},
	{L"BULLET TRAILER", BulletTrailer, ThMyShip},
	{L"BULLET TRAILER 2", BulletTrailer2, IkBlackMyShip},
	{L"EVEN OVERTAKING", EvenOvertaking, ThMyShip},
	{L"EVEN OVERTAKING 2", EvenOvertaking2, ThMyShip},
	{L"EVEN OVERTAKING 3", EvenOvertaking3, ThMyShip},
	{L"EVEN OVERTAKING 4", EvenOvertaking4, ThMyShip},
	{L"GAP", Gap, ThMyShip},
	{L"GAP 2", Gap2, ThMyShip},
	{L"GAP 3", Gap3, ThMyShip},
	{L"PATTERN", Pattern, ThMyShip},
	{L"PATTERN 2", Pattern2, ThMyShip},
	{L"STEPPING", Stepping, ThMyShip},
	{L"STEPPING 2", Stepping2, ThMyShip},
	{L"AIMING AGAIN", AimingAgain, ThMyShip},
	{L"AIMING AGAIN 2", AimingAgain2, ThMyShip},
	{L"AIMING AGAIN 3", AimingAgain3, ThMyShip},
	{L"ANY", Any, ThMyShip},
	{L"ANY 2", Any2, ThMyShip},
	{L"SPLITTING", Splitting, ThMyShip},
	{L"SPLITTING N-WAY", SplittingNWay, ThMyShip},
	{L"PLACED", Placed, ThMyShip},
	{L"SPIRAL PLACED", SpiralPlaced, ThMyShip},
	{L"SPIRAL PLACED 2", SpiralPlaced2, ThMyShip},
	{L"SPIRAL PLACED 3", SpiralPlaced3, ThMyShip},
	{L"STAR PLACED", StarPlaced, ThMyShip},
	{L"STAR PLACED 2", StarPlaced2, ThMyShip},
	{L"STAR PLACED TWICE", StarPlacedTwice, ThMyShip},
	{L"STAR PLACED TWICE 2", StarPlacedTwice2, ThMyShip},
};
size_t StageCount=sizeof(Stage)/sizeof(STAGE);

//==============================================================
// グローバル変数
CGraphics* Graphics;
CTaskList* MyShipList;
CTaskList* EnemyList;
CTaskList* BulletList;
CMyShip* MyShip;

CInputState InputState;
CRand Rand;

bool ShowHit;
bool ShowColor;

