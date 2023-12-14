#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

//TODO GameObject
#pragma region Camera
#include "Camera_Dynamic.h"
#pragma endregion

#pragma region BackGround
#include "BackGround.h"
#pragma endregion

#pragma region Terrain
#include "Terrain.h"
#include "VIBuffer_Dynamic_Terrain.h"
#include "Dynamic_Terrain.h"
#pragma endregion

#pragma region Tool
#include "Imgui_Manager.h"
#include "Anything_Object.h"
#pragma endregion

#pragma region Object
#include "Model.h"
#include "Monster.h"
#pragma endregion

#pragma region 
#pragma endregion

#include <process.h>

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

// typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);

_uint APIENTRY LoadingThread(void* pArg)
{
	//! 쓰레드가 다른 임계메모리영역을 침범해서 생기는 에러를 막기위해 컴객체를 초기화해준다.
	//! 디바이스 컴객체들은 간혹가다 다른 스레드에서 사용중인 컴객체를 사용하는 경우가 있어 알수없는 버그를 일으킨다.
	//! 그래서 특정 스레드가 시작될시 DX에서 셋팅해놨던 컴객체들을 이 스레드에서 사용할 수 있도록 다른 스레드에서 사용 중 이었다면 전부 꺼내서 초기화 해주는 함수이다.
	
	//! 대표적으로 여기서 많은 에러를 일으켰다, 텍스처를 불러올때 첫번째 이후 텍스처를 로드할 때 터지는 현상이 많이 발생했었다.
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	/* 어떤 레벨의 자원을 로드해야하는지? */
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* 스레드를 생성한다. */
	/* LoadingThread : 생성한 스레드가 가장 먼저 호출해야할 함수 */
	/* this : 위 함수를 호출하면서 인자로 전달할 데이터. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

void CLoader::Print_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT hr = 0;

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo_Level();
		break;

	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay_Level();
		break;

	case LEVEL_TOOL:
		hr = Loading_For_Tool_Level();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo_Level()
{
	/* 로고 레벨에 필요한 자원을 로드하자. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));

	//! For.Prototype_Component_Texture_Logo
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default0.jpg")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));

	lstrcpy(m_szLoadingText, TEXT("원형객체를(을) 로드하는 중입니다."));

	//! For.Prototype_GameObject_BackGround
	if(FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice,m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	return Loading_For_Level(LEVEL_GAMEPLAY);
}


HRESULT CLoader::Loading_For_Tool_Level()
{
	return Loading_For_Level(LEVEL_TOOL);
}

HRESULT CLoader::Loading_For_Level(LEVEL eLEVEL)
{
	/* 게임플레이 레벨에 필요한 자원을 로드하자. */

#pragma region 텍스쳐
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Mask */ // 마스크 추가
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Brush */ // 브러쉬 추가
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region 모델
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain */ // 다이나믹 터레인 컴포넌트
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Dynamic_Terrain"),
		CVIBuffer_Dynamic_Terrain::Create(m_pDevice, m_pContext))))	// 높이맵 x
		return E_FAIL;

	_matrix		PivotMatrix; // 내가 원하는 세팅을 하고싶어서 만든 PivotMatrix. 이 값(뼈 정보 세팅)을 모델 생성까지 전달해, 생성할때 원하는 형태로 세팅할것이다.

	/* For.Prototype_Component_Model_Fiona */ // 
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PivotMatrix)))) // 텍스처는 메쉬마다 한장만 추가한다. (string으로 넘겨주기 때문에 TEXT매크로 사용x)
		return E_FAIL;

	///* For.Prototype_Component_Model_Fiona */ // 
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_AntWorker"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AntWorker/AntWorker.fbx", PivotMatrix)))) // 텍스처는 메쉬마다 한장만 추가한다. (string으로 넘겨주기 때문에 TEXT매크로 사용x)
	//	return E_FAIL;

	/* For.Prototype_Component_Model_Fiona */ // @ 수업용 테스트 모델 피오나 @ 애니메이션o
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AntWorker/AS_Ant_Angry_01.fbx", PivotMatrix)))) // 텍스처는 메쉬마다 한장만 추가한다. (string으로 넘겨주기 때문에 TEXT매크로 사용x)
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Fiona */ // @ 수업용 테스트 모델 피오나 @ 애니메이션o
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Burgle"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Burgle/Burgle.fbx", PivotMatrix)))) // 텍스처는 메쉬마다 한장만 추가한다. (string으로 넘겨주기 때문에 TEXT매크로 사용x)
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Fiona */ // @ 수업용 테스트 모델 피오나 @ 애니메이션o
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Firefly"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Firefly/Firefly.fbx", PivotMatrix)))) // 텍스처는 메쉬마다 한장만 추가한다. (string으로 넘겨주기 때문에 TEXT매크로 사용x)
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Fiona */ // @ 수업용 테스트 모델 피오나 @ 애니메이션o
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Ladybug"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ladybug/Ladybug.fbx", PivotMatrix)))) // 텍스처는 메쉬마다 한장만 추가한다. (string으로 넘겨주기 때문에 TEXT매크로 사용x)
	//	return E_FAIL;
#pragma region NONANIM
	/* For.Prototype_Component_Model_ForkLift */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Mannequin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Mannequin/Mannequin.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ForkLift */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_CardBoardBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/CardBoardBox/CardBoardBox.fbx", PivotMatrix))))
		return E_FAIL;

	///* For.Prototype_Component_Model_ForkLift */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Garden"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Garden/Garden.fbx", PivotMatrix))))
	//	return E_FAIL;
#pragma endregion

#pragma region 셰이더
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	/* For.Prototype_Component_Shader_VtxNorTex */	// 지형 셰이더 파일
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */ // 모델 전용 셰이더 파일을 새로 추가했다. 모델은 탄젠트라는 정보가 새로 추가되어, 셰이더 파일을 새로만들어 사용한다. / 애니메이션x
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */ // 애니메이션o
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;
#pragma endregion

#pragma  region	네비게이션
	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	/* For.Prototype_Component_Navigation */ // 아래 경로에 저장해둔 네비게이션 파일을 불러와 생성한다.
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	//	return E_FAIL;
#pragma endregion

#pragma region 원형 객체
	lstrcpy(m_szLoadingText, TEXT("원형객체를(을) 로드하는 중입니다."));
	/* For.Prototype_GameObject_Terrain */	// 터레인
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dynamic_Terrain */ // 다이나믹 터레인
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_Terrain"),
		CDynamic_Terrain::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Dynamic */	// 다이나믹 카메라
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */	// 몬스터
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

	/* ==================툴 오브젝트================== */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Anything"),
		CAnything_Object::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
