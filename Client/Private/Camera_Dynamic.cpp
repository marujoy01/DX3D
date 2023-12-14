#include "stdafx.h"
#include "Camera_Dynamic.h"
#include "GameInstance.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice,pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic& rhs)
	:CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void* pArg)
{
	if(nullptr == pArg)
		return E_FAIL;
	
	DYNAMIC_CAMERA_DESC* pDesc = (DYNAMIC_CAMERA_DESC*)pArg;

	m_fMouseSensitivity = pDesc->fMouseSensitivity;

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Dynamic::Priority_Tick(_float fTimeDelta)
{
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	
	if (m_pGameInstance->Mouse_Pressing(CInput_Device::DIM_RB))
	{
		Mouse_Move(fTimeDelta);
	}

	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		if (m_bMouseFix)
		{
			m_bMouseFix = false;
		}

		if (!m_bMouseFix)
		{
			m_bMouseFix = true;
		}
	}
	
	if (m_bMouseFix)
	{
		Mouse_Fix();
	}

	//TODO 부모의 Tick함수를 호출해줘야 뷰투영행렬을 파이프라인 객체에게 던져준다.
	__super::Tick(fTimeDelta);
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
}

void CCamera_Dynamic::Mouse_Move(_float fTimeDelta)
{
	_long	MouseMove = 0;

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensitivity * MouseMove * fTimeDelta);
	}

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensitivity * MouseMove * fTimeDelta);
	}
}

void CCamera_Dynamic::Mouse_Fix()
{
	POINT	pt{ g_iWinSizeX >> 1,g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

CCamera_Dynamic* CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CCamera_Dynamic* pInstance = new CCamera_Dynamic(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic* pInstance = new CCamera_Dynamic(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();
}
