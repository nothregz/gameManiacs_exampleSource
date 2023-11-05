#include "Main.h"
#include "Stage.h"
#include "Stage1.h"
#include "Stage2.h"
#include "Stage3.h"
#include "Stage4.h"
#include "Stage5.h"
#include "Stage6.h"
#include "Stage7.h"
#include "StageExtra.h"

//==============================================================
// 전체 스테이지 작성
void MakeStage() {

	// 공격
	Game->Stage.push_back(new CShakeLeverStage());
	Game->Stage.push_back(new CBackAttackStage());
	Game->Stage.push_back(new CAttackRangeStage());
	Game->Stage.push_back(new CComboCountStage());
	Game->Stage.push_back(new CDroppingObjectStage());
	Game->Stage.push_back(new CImpactAttackStage());
	Game->Stage.push_back(new CChargeAttackStage());
	Game->Stage.push_back(new CAllRangeAttackStage());
	Game->Stage.push_back(new CTrapStage());
	Game->Stage.push_back(new CStickingAttackStage());
	Game->Stage.push_back(new CGrabAttackStage());
	Game->Stage.push_back(new CJumpAttackStage());
	Game->Stage.push_back(new CBodyAttackStage());

	// 아이템
	Game->Stage.push_back(new CItemByAttackStage());
	Game->Stage.push_back(new CItemByPassStage());
	Game->Stage.push_back(new CFallingItemStage());
	Game->Stage.push_back(new CPickAndThrowItemStage());
	Game->Stage.push_back(new CInvincibleItemStage());

	// 무기
	Game->Stage.push_back(new CChangeWeaponStage());
	Game->Stage.push_back(new CBubbleStage());
	Game->Stage.push_back(new CSmokeStage());
	Game->Stage.push_back(new CShieldStage());
	Game->Stage.push_back(new CRemoteControlStage());
	Game->Stage.push_back(new CBoomerangStage());
	Game->Stage.push_back(new CHomingMissileStage());
	Game->Stage.push_back(new CMachineGunStage());
	Game->Stage.push_back(new CBlastStage());
	Game->Stage.push_back(new CTimeBombStage());
	Game->Stage.push_back(new CGrenadeStage());
	Game->Stage.push_back(new CBouncingBallStage());
	Game->Stage.push_back(new CWhipStage());
	Game->Stage.push_back(new CSwordStage());

	// 특수 행동
	Game->Stage.push_back(new CMultipleCharacterStage());
	Game->Stage.push_back(new CGiantStage());
	Game->Stage.push_back(new CCurlUpStage());
	Game->Stage.push_back(new CCrouchStage());
	Game->Stage.push_back(new CPendulumStage());
	Game->Stage.push_back(new CSeesawStage());
	Game->Stage.push_back(new CAnimalStage());
	Game->Stage.push_back(new CCarStage());
	Game->Stage.push_back(new CSkateboardStage());

	// 지형 이용
	Game->Stage.push_back(new CDropLandStage());
	Game->Stage.push_back(new CFlappingPanelStage());
	Game->Stage.push_back(new CMakeFootholdStage());
	Game->Stage.push_back(new CPlaceRopeStage());
	Game->Stage.push_back(new CManualHoleStage());
	Game->Stage.push_back(new CAutoHoleStage());
	Game->Stage.push_back(new CPushIceStage());
	Game->Stage.push_back(new CPushObjectStage());
	Game->Stage.push_back(new CDropRockStage());
	Game->Stage.push_back(new CHangOnWallStage());

	// 장치
	Game->Stage.push_back(new CBreakWallStage());
	Game->Stage.push_back(new CSwitchingPathStage());
	Game->Stage.push_back(new CWarpGateStage());
	Game->Stage.push_back(new CAirCurrentStage());
	Game->Stage.push_back(new CConveyorStage());
	Game->Stage.push_back(new CMovingFloorStage());
	Game->Stage.push_back(new CElevatorStage());
	Game->Stage.push_back(new CDoorConfinementStage());
	Game->Stage.push_back(new CDoorSmashStage());
	Game->Stage.push_back(new CRevolvingDoorStage());
	Game->Stage.push_back(new CDroppingFloorStage());
	Game->Stage.push_back(new CTrampolineStage());
	Game->Stage.push_back(new CLadderStage());
	Game->Stage.push_back(new CRopeStage());

	// 점프
	Game->Stage.push_back(new CStampBoardStage());
	Game->Stage.push_back(new CStompStage());
	Game->Stage.push_back(new CFloorAttackStage());
	Game->Stage.push_back(new CJumpAngleStage());
	Game->Stage.push_back(new CJumpFlyStage());
	Game->Stage.push_back(new CJumpOffStage());
	Game->Stage.push_back(new CTriangleJumpStage());
	Game->Stage.push_back(new CDoubleJumpStage());
	Game->Stage.push_back(new CVariableJumpStage());
	Game->Stage.push_back(new CFixedJumpStage());

	// 이동
	Game->Stage.push_back(new CLoopStage());
	Game->Stage.push_back(new CReverseDirectionStage());
	Game->Stage.push_back(new CDecreaseLifeStage());
	Game->Stage.push_back(new CScreenEdgeWarpStage());
	Game->Stage.push_back(new CLineMoveStage());
	Game->Stage.push_back(new CSwimmingStage());
	Game->Stage.push_back(new CSlipOnIceStage());
	Game->Stage.push_back(new CSpeedUpItemStage());
	Game->Stage.push_back(new CRapidButtonDashStage());
	Game->Stage.push_back(new CDoubleLeverDashStage());
	Game->Stage.push_back(new CButtonDashStage());
	Game->Stage.push_back(new CLeverDashStage());
}

