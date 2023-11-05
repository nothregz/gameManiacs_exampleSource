#pragma warning(disable:4819)
#include "AnimMesh.h"
#include "..\..\LibUtil\LibUtil\LibUtil.h"


//==============================================================
// AllocateHierarchy을 실행할 때 사용하는 로컬 변수
static LPDIRECT3DDEVICE9 Device;
static string Path;
static CAllocateHierarchy* AllocateHierarchy=NULL;


//==============================================================
// 정점 셰이더에 의한 스키닝

// 효과(이펙트)
static ID3DXEffect* AnimMeshEffect=NULL;

// 이펙트 로드
static void LoadAnimMeshEffect(LPDIRECT3DDEVICE9 device) {
	D3DXCreateEffectFromFile(
		device, 
		(GetExePath()+"..\\Effect\\AnimMesh.fx").c_str(),
		NULL, NULL, D3DXFX_NOT_CLONEABLE, 
		NULL, &AnimMeshEffect, NULL);
}


//==============================================================
// 프레임의 생성 및 소멸

// 프레임을 생성할 때 호출되는 함수
HRESULT CAllocateHierarchy::CreateFrame(
	LPCTSTR name, LPD3DXFRAME *new_frame
) {
	*new_frame=new CFrame(name);
	return S_OK;
}

// 프레임을 파기할 때 호출되는 함수
HRESULT CAllocateHierarchy::DestroyFrame(
	LPD3DXFRAME frame
) {
	delete (CFrame*)frame;
	return S_OK;
}

// 프레임 생성자
CFrame::CFrame(LPCTSTR name) {
	ZeroMemory(this, sizeof(D3DXFRAME));

	// 기본 클래스(구조체)의 초기화
	pMeshContainer=NULL;
	pFrameSibling=NULL;
	pFrameFirstChild=NULL;

	// 프레임 이름 설정
	if (name) {
		int n=lstrlen(name)+1;
		Name=(LPSTR)new TCHAR[n];
		memcpy(Name, name, n);
	} else {
		Name="";
	}

	// 행렬
	D3DXMatrixIdentity(&TransformationMatrix);
	D3DXMatrixIdentity(&CombinedTransMatrix);
}

// 프레임의 소멸자
CFrame::~CFrame() {
	if (MeshContainer) delete MeshContainer;
	if (Sibling) delete Sibling;
	if (Child) delete Child;
}


//==============================================================
// 메쉬 컨테이너의 생성 및 소멸
// 메쉬 컨테이너가 생성될 때 호출되는 함수

HRESULT CAllocateHierarchy::CreateMeshContainer(
	THIS_ LPCSTR name, 
	CONST D3DXMESHDATA *mesh_data,
	CONST D3DXMATERIAL *materials, 
	CONST D3DXEFFECTINSTANCE *effect_instances,
	DWORD num_materials, 
	CONST DWORD *adjacency, 
	LPD3DXSKININFO skin_info, 
	LPD3DXMESHCONTAINER *new_mesh_container
) {
	*new_mesh_container=new CMeshContainer(
		(LPCTSTR)name, mesh_data, materials, num_materials,
		skin_info, adjacency);
	return S_OK;
}

// 메쉬 컨테이너가 파기될 때 호출되는 함수
HRESULT CAllocateHierarchy::DestroyMeshContainer(
	LPD3DXMESHCONTAINER mesh_container
) {
	delete (CMeshContainer*)mesh_container;
	return S_OK;
}

// 메쉬 컨테이너의 생성자
CMeshContainer::CMeshContainer(
	THIS_ LPCTSTR name,
	CONST D3DXMESHDATA *mesh_data,
	CONST D3DXMATERIAL *materials,
	DWORD num_materials,
	LPD3DXSKININFO skin_info,
	CONST DWORD *adjacency
)
:	Mesh(mesh_data->pMesh), NumMaterials(num_materials), 
	Materials(NULL), Textures(NULL), 
	SkinInfo(skin_info), NumBones(0), BoneOffsetMatrices(0),
	VPMode(VP_SOFTWARE)
{
	ZeroMemory(this, sizeof(D3DXMESHCONTAINER));

	// 메쉬 컨테이너 이름 설정
	if (name) {
		int n=lstrlen(name)+1;
		Name=(LPSTR)new TCHAR[n];
		memcpy(Name, name, n);
	} else {
		Name="";
	}

	// 메쉬 로드
	if (!(Mesh->GetFVF()&D3DFVF_NORMAL)) {
		Mesh->CloneMeshFVF(
			Mesh->GetOptions(), 
			Mesh->GetFVF() | D3DFVF_NORMAL, 
			Device, &Mesh);
		D3DXComputeNormals(Mesh, NULL);
	} else {
		Mesh->AddRef();
	}

	// 재질(material) 및 텍스처 로드
	Materials=new D3DMATERIAL9[NumMaterials];
	Textures=new LPDIRECT3DTEXTURE9[NumMaterials];
	for (DWORD i=0; i<NumMaterials; i++ ) {

		// 재질(material) 복사
		Materials[i]=materials[i].MatD3D;

		// 환경(ambient) 색 설정
		Materials[i].Ambient=Materials[i].Diffuse;

		// 텍스처 로드
		Textures[i]=NULL;
		char* texture_file=materials[i].pTextureFilename;
		if (texture_file && lstrlen(texture_file)>0) {
			if (FAILED(D3DXCreateTextureFromFile(
				Device, (Path+texture_file).c_str(), 
				&Textures[i]))) 
				continue;
		}
	}

	// 스키닝 정보 로드
	if (SkinInfo) {
		SkinInfo->AddRef();
		
		// 본(bone) 변형 행렬, 본(bone offset) 행렬 작성 
		NumBones=SkinInfo->GetNumBones();
		BoneTransMatrices=new D3DXMATRIX*[NumBones];
		BoneOffsetMatrices=new D3DXMATRIX[NumBones];
		for (DWORD i=0; i<NumBones; i++) {
			BoneOffsetMatrices[i]=
				*SkinInfo->GetBoneOffsetMatrix(i);
		}

		// 동시에 사용하는 bone 행렬의 최대수 구하기
		LPDIRECT3DINDEXBUFFER9 ib=NULL;
		Mesh->GetIndexBuffer(&ib);
		SkinInfo->GetMaxFaceInfluences(
			ib, Mesh->GetNumFaces(), &NumMaxInfls);
		ib->Release();
		NumMaxInfls=min(NumMaxInfls, 12);

		// 인덱스 정점 블렌딩(혼합)에서
		// 하드웨어를 지원하는지 확인
		DWORD flags=D3DXMESHOPT_VERTEXCACHE;
		D3DCAPS9 caps;
		Device->GetDeviceCaps(&caps);
		if (caps.MaxVertexBlendMatrixIndex+1<NumMaxInfls) {
			
			// 소프트웨어를 사용하는 경우
			NumPaletteEntries=min(256, NumBones);
			VPMode=VP_SOFTWARE;
			flags|=D3DXMESH_SYSTEMMEM;

		} else {

			// 하드웨어를 사용하는 경우
			NumPaletteEntries=min(
				(caps.MaxVertexBlendMatrixIndex+1)/2, 
				NumBones);
			VPMode=VP_INDEXED;
			flags|=D3DXMESH_MANAGED;
		}

		// 스킨 대응 메쉬 작성
		SkinInfo->ConvertToIndexedBlendedMesh(
			Mesh, flags, NumPaletteEntries, adjacency, 
			NULL, NULL, NULL, &NumMaxInfls, 
			&NumBoneCombinations, &BoneCombinations, 
			&Mesh);
	}


/*	{
		// Get palette size
		// First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
		// (96 - 9) /3 i.e. Maximum constant count - used constants 
		UINT MaxMatrices = 26; 
		pMeshContainer->NumPaletteEntries = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

		DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
		if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
		{
			pMeshContainer->UseSoftwareVP = false;
			Flags |= D3DXMESH_MANAGED;
		}
		else
		{
			pMeshContainer->UseSoftwareVP = true;
			g_bUseSoftwareVP = true;
			Flags |= D3DXMESH_SYSTEMMEM;
		}

		SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
												(
												pMeshContainer->pOrigMesh,
												Flags, 
												pMeshContainer->NumPaletteEntries, 
												pMeshContainer->pAdjacency, 
												NULL, NULL, NULL,			 
												&pMeshContainer->NumInfl,
												&pMeshContainer->NumAttributeGroups, 
												&pMeshContainer->pBoneCombinationBuf, 
												&pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;


		// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
		DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
		if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
		{
			LPD3DXMESH pMesh;
			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
			if (!FAILED(hr))
			{
				pMeshContainer->MeshData.pMesh->Release();
				pMeshContainer->MeshData.pMesh = pMesh;
				pMesh = NULL;
			}
		}

		D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
		LPD3DVERTEXELEMENT9 pDeclCur;
		hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
		if (FAILED(hr))
			goto e_Exit;

		// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
		//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
		//		  this is more of a "cast" operation
		pDeclCur = pDecl;
		while (pDeclCur->Stream != 0xff)
		{
			if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
				pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
			pDeclCur++;
		}

		hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
		if (FAILED(hr))
			goto e_Exit;

		// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
		if( g_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones() )
		{
			g_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

			// Allocate space for blend matrices
			delete[] g_pBoneMatrices; 
			g_pBoneMatrices  = new D3DXMATRIXA16[g_NumBoneMatricesMax];
			if( g_pBoneMatrices == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}
		}
	}
*/
}

// 메쉬 컨테이너의 소멸자
CMeshContainer::~CMeshContainer() {
	if (Mesh) Mesh->Release();
	if (Materials) delete[] Materials;
	if (Textures) {
		for (DWORD i=0; i<NumMaterials; i++) {
			if (Textures[i]) Textures[i]->Release();
		}
		delete[] Textures;
	}
	if (SkinInfo) SkinInfo->Release();
	if (BoneOffsetMatrices) delete[] BoneOffsetMatrices;
}


//==============================================================
// 프레임 그리기
void CFrame::Draw() {
	
	// 프레임에 속하는 모든 메쉬 컨테이너 그리기
	for (CMeshContainer* mc=MeshContainer; mc; mc=mc->Next()) {
		MeshContainer->Draw(this);
	}

	// 형제 프레임, 아이 프레임 그리기
	if (Sibling) Sibling->Draw();
	if (Child) Child->Draw();
}


//==============================================================
// 메쉬 컨테이너 그리기
void CMeshContainer::Draw(CFrame* frame) {

	// 스키닝 할 경우 그리기
	if (SkinInfo) {

		// 그리기 조건 설정
		if (VPMode==VP_SOFTWARE) {
			Device->SetSoftwareVertexProcessing(TRUE);
		}
		if (NumMaxInfls==1) {
			Device->SetRenderState(
				D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
		} else {
			Device->SetRenderState(
				D3DRS_VERTEXBLEND, NumMaxInfls-1);
		}
		if (NumMaxInfls) {
			Device->SetRenderState(
				D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
		}

		// bone 조합
		// (재질(material), 질감(texture), bone 행렬이
		// 일치하는 다각형 조직）
		// 별로 구분해 그리기
		LPD3DXBONECOMBINATION bc=
			reinterpret_cast<LPD3DXBONECOMBINATION>(
				BoneCombinations->GetBufferPointer());
		D3DXMATRIXA16 mat;
		for (DWORD ibc=0; ibc<NumBoneCombinations; ibc++) {

			// bone 행렬 팔레트의 설정
			for (DWORD ip=0; ip<NumPaletteEntries; ip++) {
				DWORD bone=bc[ibc].BoneId[ip];
				if (bone!=UINT_MAX) {
					
					// bone 오프셋 행렬과 bone 변형 행렬을
					// 곱하여 팔레트에 설정
					D3DXMatrixMultiply(
						&mat, &BoneOffsetMatrices[bone], 
						BoneTransMatrices[bone]);
					Device->SetTransform(
						D3DTS_WORLDMATRIX(ip), &mat);
				}
			}

			// 재질(material) 설정
			Device->SetMaterial(&Materials[bc[ibc].AttribId]);
			Device->SetTexture(0, Textures[bc[ibc].AttribId]);

			// 다각형 그리기
			Mesh->DrawSubset(ibc);
		}

		// 그리기 조건의 복귀
		Device->SetRenderState(
			D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		Device->SetRenderState(
			D3DRS_VERTEXBLEND, 0);
		if (VPMode==VP_SOFTWARE) {
			Device->SetSoftwareVertexProcessing(FALSE);
		}

/*
        else if (g_SkinningMethod == D3DINDEXEDHLSLVS) 
        {
            if (pMeshContainer->UseSoftwareVP)
            {
                // If hw or pure hw vertex processing is forced, we can't render the
                // mesh, so just exit out.  Typical applications should create
                // a device with appropriate vertex processing capability for this
                // skinning method.
                if( g_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
                    return;

                V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
            }

            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
            for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
            { 
                // first calculate all the world matrices
                for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
                {
                    iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
                    if (iMatrixIndex != UINT_MAX)
                    {
                        D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
                        D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &matTemp, &g_matView);
                    }
                }
                V( g_pEffect->SetMatrixArray( "mWorldMatrixArray", g_pBoneMatrices, pMeshContainer->NumPaletteEntries) );

                // Sum of all ambient and emissive contribution
                D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
                D3DXCOLOR color2(.25, .25, .25, 1.0);
                D3DXCOLOR ambEmm;
                D3DXColorModulate(&ambEmm, &color1, &color2);
                ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

                // set material color properties 
                V( g_pEffect->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse)) );
                V( g_pEffect->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm) );

                // setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
                V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );

                // Set CurNumBones to select the correct vertex shader for the number of bones
                V( g_pEffect->SetInt( "CurNumBones", pMeshContainer->NumInfl -1) );

                // Start the effect now all parameters have been updated
                UINT numPasses;
                V( g_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE ) );
                for( UINT iPass = 0; iPass < numPasses; iPass++ )
                {
                    V( g_pEffect->BeginPass( iPass ) );

                    // draw the subset with the current world matrix palette and material state
                    V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );

                    V( g_pEffect->EndPass() );
                }

                V( g_pEffect->End() );

                V( pd3dDevice->SetVertexShader(NULL) );
            }

            // remember to reset back to hw vertex processing if software was required
            if (pMeshContainer->UseSoftwareVP)
            {
                V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
            }
        }
*/
	}

	// 스키닝하지 않는 경우의 그리기
	else {

		// 행렬 설정
		Device->SetTransform(
			D3DTS_WORLD, frame->GetCombinedTransMatrix());

		// 재질(material), 질감(texture)이 일치하는 다각형 세트별로
		// 나누어 그리기
		for (DWORD i=0; i<NumMaterials; i++) {
			Device->SetMaterial(&Materials[i]);
			Device->SetTexture(0, Textures[i]);
			Mesh->DrawSubset(i);
		}
	}	
}


//==============================================================
// bone 변형 행렬의 설정 및 업데이트

// 프레임 검색
CFrame* CFrame::Find(LPCTSTR name) {
	return (CFrame*)D3DXFrameFind(this, (LPCSTR)name);
}

// bone 변형 행렬의 초기화(프레임)
void CFrame::InitBoneTransMatrices(CFrame* root) {
	
	// 메쉬 컨테이너, 형제프레임, 아이프레임의
	// 포인터 초기화
	MeshContainer=(CMeshContainer*)pMeshContainer;
	Sibling=(CFrame*)pFrameSibling;
	Child=(CFrame*)pFrameFirstChild;

	// 메쉬 컨테이너, 형제프레임, 아이프레임에 대한
	// bone 변형 행렬의 초기화
	if (MeshContainer) MeshContainer->InitBoneTransMatrices(root);
	if (Sibling) Sibling->InitBoneTransMatrices(root);
	if (Child) Child->InitBoneTransMatrices(root);
}

// bone 변형 행렬의 초기화 (메쉬 컨테이너)
void CMeshContainer::InitBoneTransMatrices(CFrame* root) {
	if (SkinInfo) {
		
		// 모든 뼈(bone)에 대해 해당 프레임을 찾는다
		for (DWORD i=0; i<NumBones; i++) {
			CFrame* frame=root->Find((LPCTSTR)SkinInfo->GetBoneName(i));
			
			// 대응하는 프레임의 합성된 변형행렬을
			// bone에서 참조한다
			if (frame) {
				BoneTransMatrices[i]=
					frame->GetCombinedTransMatrix();
			}
		}
	}
}

// 변형 행렬의 갱신
void CFrame::UpdateMatrices(LPD3DXMATRIX parent_mat) {

	// 이 프레임에 관한 변형 행렬의 갱신
	if (parent_mat) {
		D3DXMatrixMultiply(
			&CombinedTransMatrix, 
			&TransformationMatrix, parent_mat);
	} else {
		CombinedTransMatrix=TransformationMatrix;
	}

	// 형제프레임, 아이프레임에 대한 변형 행렬의 갱신
	if (Sibling) Sibling->UpdateMatrices(parent_mat);
	if (Child) Child->UpdateMatrices(&CombinedTransMatrix);
}


//==============================================================
// 애니메이션 대응 메쉬 (CAnimMesh 클래스)

// 생성자
CAnimMesh::CAnimMesh(LPDIRECT3DDEVICE9 device)
:	Device(device)
{
	if (!AllocateHierarchy) 
		AllocateHierarchy=new CAllocateHierarchy();
	if (!AnimMeshEffect) LoadAnimMeshEffect(device);
}

// 소멸자
CAnimMesh::~CAnimMesh() {
	if (AnimController) AnimController->Release();
	delete FrameRoot;
}

// 파일 로드
void CAnimMesh::LoadFromFile(string file) {

	// Direct3D 디바이스 및 파일 경로를 전역변수로 설정
	::Device=Device;
	::Path=ExtractFilePath(file);

	// 메쉬 로드
	LPD3DXFRAME frame_root;
	if (FAILED(D3DXLoadMeshHierarchyFromX(
		file.c_str(), D3DXMESH_MANAGED, 
		Device, AllocateHierarchy, 
		NULL, &frame_root, &AnimController))) return;
	FrameRoot=(CFrame*)frame_root;

	// bone 변형 행렬의 설정
	FrameRoot->InitBoneTransMatrices(FrameRoot);
}


//==============================================================
// 애니메이션 (CAnimMesh 클래스)

// 시간 취득
double CAnimMesh::GetAnimTime() {
	if (AnimController) return AnimController->GetTime();
	return 0;
}

// 시간 설정
void CAnimMesh::AdvanceAnimTime(double time) {
	if (AnimController) AnimController->AdvanceTime(time, NULL);
}

// 애니메이션 set 설정
void CAnimMesh::SetAnimSet(DWORD track, DWORD set) {
	if (AnimController) {
		LPD3DXANIMATIONSET aset;
		AnimController->GetAnimationSet(set, &aset);
		AnimController->SetTrackAnimationSet(track, aset);
	}
}

// 애니메이션 set수 취득
DWORD CAnimMesh::GetAnimSetCount() {
	if (AnimController) return AnimController->GetNumAnimationSets();
	return 0;
}

// 트랙 유효화
void CAnimMesh::SetAnimEnabled(DWORD track, bool enabled) {
	if (AnimController) 
		AnimController->SetTrackEnable(track, enabled);
}

// 트랙 가중치 설정
void CAnimMesh::SetAnimWeight(DWORD track, float weight) {
	if (AnimController) 
		AnimController->SetTrackWeight(track, weight);
}

// 트랙 속도 설정
void CAnimMesh::SetAnimSpeed(DWORD track, float speed) {
	if (AnimController) 
		AnimController->SetTrackSpeed(track, speed);
}

// 트랙 수 취득
DWORD CAnimMesh::GetAnimNumTracks() {
	if (AnimController) return AnimController->GetMaxNumTracks();
	return 0;
}


//==============================================================
// 그리기

// 변환 행렬을 지정하여 그리기
void CAnimMesh::Draw(LPD3DXMATRIX mat) {
	::Device=Device;
	FrameRoot->UpdateMatrices(mat);
	FrameRoot->Draw();
}

// 디폴트 행렬(단위행렬)로 그리기
void CAnimMesh::Draw() {
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	Draw(&mat);
}

#define TURN_X \
	if (tx) {\
		D3DXMatrixRotationX(&mat0, D3DX_PI*2*tx);\
		D3DXMatrixMultiply(&mat, &mat, &mat0);\
	}
#define TURN_Y \
	if (ty) {\
		D3DXMatrixRotationY(&mat0, D3DX_PI*2*ty);\
		D3DXMatrixMultiply(&mat, &mat, &mat0);\
	}
#define TURN_Z \
	if (tz) {\
		D3DXMatrixRotationZ(&mat0, D3DX_PI*2*tz);\
		D3DXMatrixMultiply(&mat, &mat, &mat0);\
	}

// 좌표, 크기, 회전을 지정하여 그리기
void CAnimMesh::Draw(
	float x, float y, float z, 
	float sx, float sy, float sz, 
	float tx, float ty, float tz, TURN_ORDER order
) {
	D3DXMATRIX mat, mat0;
	
	// 확대 축소
	D3DXMatrixScaling(&mat, sx, sy, sz);

	// 회전
	switch (order) {
		case TO_ZY: TURN_Z TURN_Y break;
		case TO_X: TURN_X break;
		case TO_Y: TURN_Y break;
		case TO_Z: TURN_Z break;
		case TO_XY: TURN_X TURN_Y break;
		case TO_XZ: TURN_X TURN_Z break;
		case TO_YX: TURN_Y TURN_X break;
		case TO_YZ: TURN_Y TURN_Z break;
		case TO_ZX: TURN_Z TURN_X break;
		case TO_XYZ: TURN_X TURN_Y TURN_Z break;
		case TO_XZY: TURN_X TURN_Z TURN_Y break;
		case TO_YXZ: TURN_Y TURN_X TURN_Z break;
		case TO_YZX: TURN_Y TURN_Z TURN_X break;
		case TO_ZXY: TURN_Z TURN_X TURN_Y break;
		case TO_ZYX: TURN_Z TURN_Y TURN_X break;
	}

	// 좌표
	D3DXMatrixTranslation(&mat0, x, y, z);
	D3DXMatrixMultiply(&mat, &mat, &mat0);
	
	// 그리기
	Device->SetTransform(D3DTS_WORLD, &mat);
	Draw(&mat);
}
