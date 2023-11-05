#ifndef ANIMMESHH
#define ANIMMESHH

#include "Mesh.h"
#include <d3dx9.h>
#include <string>
using namespace std;


class CMeshContainer;


//==============================================================
// 프레임과 메쉬 컨테이너를 로드하는 데 사용하는 
// ID3DXAllocateHierarchy 파생 클래스
class CAllocateHierarchy: public ID3DXAllocateHierarchy {
public:
	STDMETHOD(CreateFrame)(
		THIS_ LPCTSTR name, LPD3DXFRAME *new_frame);
	STDMETHOD(CreateMeshContainer)(
		THIS_ LPCSTR name, 
		CONST D3DXMESHDATA *mesh_data,
		CONST D3DXMATERIAL *materials, 
		CONST D3DXEFFECTINSTANCE *effect_instances,
		DWORD num_materials, 
		CONST DWORD *adjacency, 
		LPD3DXSKININFO skin_info, 
		LPD3DXMESHCONTAINER *new_mesh_container);
	STDMETHOD(DestroyFrame)(
		THIS_ LPD3DXFRAME frame_to_tree);
	STDMETHOD(DestroyMeshContainer)(
		THIS_ LPD3DXMESHCONTAINER mesh_container_base);
};


//==============================================================
// 프레임
class CFrame : public D3DXFRAME {
private:

	// 메쉬컨테이너, 형제프레임, 아이프레임
	CMeshContainer* MeshContainer;
	CFrame* Sibling;
	CFrame* Child;

	// 합성된 변형 행렬
	D3DXMATRIXA16 CombinedTransMatrix;

public:

	// 생성자, 소멸자
	CFrame(LPCTSTR name);
	~CFrame();

	// 행렬 취득
	LPD3DXMATRIXA16 GetCombinedTransMatrix() {
		return &CombinedTransMatrix;
	}
	
	// 프레임 검색
	CFrame* Find(LPCTSTR name);

	// bone 변형 행렬의 초기화
	void InitBoneTransMatrices(CFrame* root);

	// 변형 행렬의 갱신
	void UpdateMatrices(LPD3DXMATRIX parent_mat);

	// 그리기
	void Draw();
};


//==============================================================
// 정점 처리 모드
typedef enum {
	VP_HLSL, VP_INDEXED, VP_SOFTWARE
} VPMODE;


//==============================================================
// 메쉬 컨테이너
class CMeshContainer : public D3DXMESHCONTAINER {
private:

	// 메쉬
	ID3DXMesh* Mesh;

	// 재질(material)의 수, 재질, 텍스처
	DWORD NumMaterials;
	D3DMATERIAL9* Materials;
	LPDIRECT3DTEXTURE9* Textures;

	// 스킨 정보
	LPD3DXSKININFO SkinInfo;

	// 뼈의 수, 오프셋 행렬, 트랜스폼(변형) 행렬
	DWORD NumBones;
	D3DXMATRIX* BoneOffsetMatrices;
	D3DXMATRIX** BoneTransMatrices;

	// bone 행렬 팔레트 수, 동시에 영향을 주는 bone 최대수
    DWORD NumPaletteEntries;
	DWORD NumMaxInfls;

	// bone 조합의 수, bone 조합
	DWORD NumBoneCombinations;
	LPD3DXBUFFER BoneCombinations;

	// 정점처리모드
	VPMODE VPMode;

public:

	// 생성자
	CMeshContainer(
		THIS_ LPCTSTR name,
		CONST D3DXMESHDATA *mesh_data,
		CONST D3DXMATERIAL *materials,
		DWORD num_materials,
		LPD3DXSKININFO skin_info,
		CONST DWORD *adjacency
	);

	// 소멸자
	~CMeshContainer();

	// 다음 메쉬 컨테이너를 취득
	CMeshContainer* Next() {
		return (CMeshContainer*)pNextMeshContainer;
	}

	// bone 변형 행렬의 초기화
	void InitBoneTransMatrices(CFrame* root);

	// 그리기
	void Draw(CFrame* frame);
};


//==============================================================
// 애니메이션 대응 메쉬
class CAnimMesh {
private:

	// 디바이스
	LPDIRECT3DDEVICE9 Device;

	// 루트 프레임
	CFrame* FrameRoot;

	// 애니메이션 컨트롤러
	LPD3DXANIMATIONCONTROLLER AnimController;

public:

	// 생성자, 소멸자
	CAnimMesh(LPDIRECT3DDEVICE9 device);
	~CAnimMesh();

	// 파일 가져오기
	void LoadFromFile(string file);

	// 애니메이션
	double GetAnimTime();
	void AdvanceAnimTime(double time);
	void SetAnimSet(DWORD track, DWORD set);
	void SetAnimEnabled(DWORD track, bool enabled);
	void SetAnimWeight(DWORD track, float weight);
	void SetAnimSpeed(DWORD track, float speed);
	DWORD GetAnimNumTracks();
	DWORD GetAnimSetCount();

	// 프레임 취득
	CFrame* GetFrameRoot() { return FrameRoot; }

	// 그리기
	void Draw(LPD3DXMATRIX mat);
	void Draw();
	void Draw(
		float x, float y, float z, 
		float sx, float sy, float sz, 
		float tx, float ty, float tz, TURN_ORDER order);
};


#endif
