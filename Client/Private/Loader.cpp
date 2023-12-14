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
	//! �����尡 �ٸ� �Ӱ�޸𸮿����� ħ���ؼ� ����� ������ �������� �İ�ü�� �ʱ�ȭ���ش�.
	//! ����̽� �İ�ü���� ��Ȥ���� �ٸ� �����忡�� ������� �İ�ü�� ����ϴ� ��찡 �־� �˼����� ���׸� ����Ų��.
	//! �׷��� Ư�� �����尡 ���۵ɽ� DX���� �����س��� �İ�ü���� �� �����忡�� ����� �� �ֵ��� �ٸ� �����忡�� ��� �� �̾��ٸ� ���� ������ �ʱ�ȭ ���ִ� �Լ��̴�.
	
	//! ��ǥ������ ���⼭ ���� ������ �����״�, �ؽ�ó�� �ҷ��ö� ù��° ���� �ؽ�ó�� �ε��� �� ������ ������ ���� �߻��߾���.
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	/* � ������ �ڿ��� �ε��ؾ��ϴ���? */
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* �����带 �����Ѵ�. */
	/* LoadingThread : ������ �����尡 ���� ���� ȣ���ؾ��� �Լ� */
	/* this : �� �Լ��� ȣ���ϸ鼭 ���ڷ� ������ ������. */
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
	/* �ΰ� ������ �ʿ��� �ڿ��� �ε�����. */
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε��ϴ� ���Դϴ�."));

	//! For.Prototype_Component_Texture_Logo
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default0.jpg")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�𵨸�(��) �ε��ϴ� ���Դϴ�."));

	lstrcpy(m_szLoadingText, TEXT("���̴���(��) �ε��ϴ� ���Դϴ�."));

	lstrcpy(m_szLoadingText, TEXT("������ü��(��) �ε��ϴ� ���Դϴ�."));

	//! For.Prototype_GameObject_BackGround
	if(FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice,m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

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
	/* �����÷��� ������ �ʿ��� �ڿ��� �ε�����. */

#pragma region �ؽ���
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε��ϴ� ���Դϴ�."));
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Mask */ // ����ũ �߰�
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Brush */ // �귯�� �߰�
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region ��
	lstrcpy(m_szLoadingText, TEXT("�𵨸�(��) �ε��ϴ� ���Դϴ�."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain */ // ���̳��� �ͷ��� ������Ʈ
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Dynamic_Terrain"),
		CVIBuffer_Dynamic_Terrain::Create(m_pDevice, m_pContext))))	// ���̸� x
		return E_FAIL;

	_matrix		PivotMatrix; // ���� ���ϴ� ������ �ϰ�; ���� PivotMatrix. �� ��(�� ���� ����)�� �� �������� ������, �����Ҷ� ���ϴ� ���·� �����Ұ��̴�.

	/* For.Prototype_Component_Model_Fiona */ // 
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PivotMatrix)))) // �ؽ�ó�� �޽����� ���常 �߰��Ѵ�. (string���� �Ѱ��ֱ� ������ TEXT��ũ�� ���x)
		return E_FAIL;

	///* For.Prototype_Component_Model_Fiona */ // 
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_AntWorker"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AntWorker/AntWorker.fbx", PivotMatrix)))) // �ؽ�ó�� �޽����� ���常 �߰��Ѵ�. (string���� �Ѱ��ֱ� ������ TEXT��ũ�� ���x)
	//	return E_FAIL;

	/* For.Prototype_Component_Model_Fiona */ // @ ������ �׽�Ʈ �� �ǿ��� @ �ִϸ��̼�o
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AntWorker/AS_Ant_Angry_01.fbx", PivotMatrix)))) // �ؽ�ó�� �޽����� ���常 �߰��Ѵ�. (string���� �Ѱ��ֱ� ������ TEXT��ũ�� ���x)
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Fiona */ // @ ������ �׽�Ʈ �� �ǿ��� @ �ִϸ��̼�o
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Burgle"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Burgle/Burgle.fbx", PivotMatrix)))) // �ؽ�ó�� �޽����� ���常 �߰��Ѵ�. (string���� �Ѱ��ֱ� ������ TEXT��ũ�� ���x)
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Fiona */ // @ ������ �׽�Ʈ �� �ǿ��� @ �ִϸ��̼�o
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Firefly"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Firefly/Firefly.fbx", PivotMatrix)))) // �ؽ�ó�� �޽����� ���常 �߰��Ѵ�. (string���� �Ѱ��ֱ� ������ TEXT��ũ�� ���x)
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Fiona */ // @ ������ �׽�Ʈ �� �ǿ��� @ �ִϸ��̼�o
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Ladybug"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ladybug/Ladybug.fbx", PivotMatrix)))) // �ؽ�ó�� �޽����� ���常 �߰��Ѵ�. (string���� �Ѱ��ֱ� ������ TEXT��ũ�� ���x)
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

#pragma region ���̴�
	lstrcpy(m_szLoadingText, TEXT("���̴���(��) �ε��ϴ� ���Դϴ�."));
	/* For.Prototype_Component_Shader_VtxNorTex */	// ���� ���̴� ����
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */ // �� ���� ���̴� ������ ���� �߰��ߴ�. ���� ź��Ʈ��� ������ ���� �߰��Ǿ�, ���̴� ������ ���θ���� ����Ѵ�. / �ִϸ��̼�x
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */ // �ִϸ��̼�o
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;
#pragma endregion

#pragma  region	�׺���̼�
	lstrcpy(m_szLoadingText, TEXT("�׺���̼Ǹ�(��) �ε��ϴ� ���Դϴ�."));
	/* For.Prototype_Component_Navigation */ // �Ʒ� ��ο� �����ص� �׺���̼� ������ �ҷ��� �����Ѵ�.
	if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	//	return E_FAIL;
#pragma endregion

#pragma region ���� ��ü
	lstrcpy(m_szLoadingText, TEXT("������ü��(��) �ε��ϴ� ���Դϴ�."));
	/* For.Prototype_GameObject_Terrain */	// �ͷ���
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dynamic_Terrain */ // ���̳��� �ͷ���
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_Terrain"),
		CDynamic_Terrain::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Dynamic */	// ���̳��� ī�޶�
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */	// ����
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

	/* ==================�� ������Ʈ================== */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Anything"),
		CAnything_Object::Create(m_pDevice, m_pContext, eLEVEL))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

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
