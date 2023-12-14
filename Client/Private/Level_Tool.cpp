#include "stdafx.h"
#include "Level_Tool.h"

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_win32.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "Imgui_Manager.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;	

	if (FAILED(Ready_Imgui()))
	{
		Safe_Release(m_pDevice);
		Safe_Release(m_pContext);

		return E_FAIL;
	}
 	
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		if(GetAsyncKeyState(VK_F5) & 0x8000)
			if (!m_bModeChange)
			{
				m_pGameInstance->UseFullScreen(true);
			}
			else
			{
				m_pGameInstance->UseFullScreen(false);
			}

			m_bModeChange = !m_bModeChange;
	}
	
		/*if (GetAsyncKeyState(VK_MENU) & 0x8000)
		{
			if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			{
				if (!m_bModeChange)
				{
					m_pGameInstance->UseFullScreen(true);
				}
				else
				{
					m_pGameInstance->UseFullScreen(false);
				}

				m_bModeChange = !m_bModeChange;
				
			}
		}*/

}

HRESULT CLevel_Tool::Render()
{
	SetWindowText(g_hWnd, TEXT("툴 레벨입니다."));

	if (m_pImguiManager->Get_Ready())
	{
		m_pImguiManager->Tick();
		m_pImguiManager->Render();
	}

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Imgui()
{
	m_pImguiManager = CImgui_Manager::GetInstance();
	m_pImguiManager->AddRef();
	
	if(nullptr == m_pImguiManager)
		return E_FAIL;

	if(FAILED(m_pImguiManager->Initialize(m_pDevice,m_pContext)))
		return E_FAIL;

	m_pImguiManager->Set_Ready(true);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Dynamic::DYNAMIC_CAMERA_DESC	Desc = {};

	Desc.fMouseSensitivity = 0.03f;
	Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovY = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 20.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const wstring& strLayerTag) // 레벨에서 생성안하고 툴에서 버튼 누르면 생성하게 하기.
{
	if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Dynamic_Terrain"))))
		return E_FAIL;

	return S_OK;
}



CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tool::Free()
{
	Safe_Release(m_pImguiManager);
	__super::Free();
	CImgui_Manager::GetInstance()->DestroyInstance();
}
