#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	//TODO 그래픽 디바이스 초기화
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//TODO 타이머 셋팅
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	//TODO 인풋디바이스 셋팅
	m_pInput_Device = CInput_Device::Create(m_hInst, m_hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	//TODO 레벨 셋팅
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;
	
	//TODO 오브젝트 매니저
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	//TODO 컴포넌트 매니저
	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	//TODO 렌더러
	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	//TODO 파이프라인
	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_tGraphicDesc = GraphicDesc;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if(nullptr == m_pLevel_Manager ||
	   nullptr == m_pObject_Manager ||
	   nullptr == m_pPipeLine ||
	   nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Tick();

	m_pObject_Manager->Priority_Tick(fTimeDelta);
	
	m_pObject_Manager->Tick(fTimeDelta);

	//TODO 파이프라인 틱 호출순서에 관하여
	//! 카메라는 오브젝트 매니저에 들어갈것이다. 카메라의 Tick함수에서 카메라의 뷰, 투영 행렬을 생성해주기 때문에
	//! 카메라의 뷰, 투영행렬을 가지고있을 파이프라인의 Tick함수는 카메라의 Tick함수 이후에 해주어야 한다.
	m_pPipeLine->Tick();
	//! 객체들의 Late_Tick에서 파이프라인에게 접근해서 작업할 수 있기에 Tick과 Late_Tick 사이에 넣은 것이다.

	m_pObject_Manager->Late_Tick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);

	m_pInput_Device->LateTick();
}



HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pRenderer->Draw_RenderGroup();

#ifdef _DEBUG
	m_pLevel_Manager->Render();
#endif

	return S_OK;
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if(nullptr == m_pObject_Manager
	 || nullptr == m_pComponent_Manager)
		return;

	//! 오브젝트 매니져에 레벨별로 구분 해 놓은 객체들 중 특정된 객체들을 지운다.
	m_pObject_Manager->Clear(iLevelIndex);

	//! 컴포넌트 매니져에 레벨별로 구분 해 놓은 컴포넌트들 중 특정된 객체들을 지운다.
	m_pComponent_Manager->Clear(iLevelIndex);
}



HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::UseFullScreen(_bool bMode)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->UseFullScreen(bMode);
}

HRESULT CGameInstance::Resize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if(nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Resize(hWnd, message, wParam, lParam);
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimeTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimeTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring& strTimeTag)
{
	if(nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimeTag);
}

HRESULT CGameInstance::Open_Level(_uint iCurrentLevelIndex, CLevel* pNewLevel)
{
	if(nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iCurrentLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if(nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag,pPrototype);
}

HRESULT CGameInstance::Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg, CGameObject** ppOut)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg, ppOut);
}

map<const wstring, class CGameObject*>* CGameInstance::Get_GameObject()
{
	return m_pObject_Manager->Get_GameObject();
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex,strPrototypeTag,pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag,pArg);
}

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, CGameObject* pGameObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(eGroupID, pGameObject);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState,TransformMatrix);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrixInverse(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4Inverse(eState);
}

RAY CGameInstance::Get_MouseRay_World(const _uint& In_ViewPortWidth, const _uint& In_ViewPortHeight)
{
	if (nullptr == m_pPipeLine)
		return RAY();

	return m_pPipeLine->Get_MouseRay_World(In_ViewPortWidth, In_ViewPortHeight);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_hWnd_hInst(HINSTANCE hInst, HWND hWnd)
{
	m_hInst = hInst;
	m_hWnd = hWnd;
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return _byte();

	return m_pInput_Device->Get_DIKeyState(byKeyID);

}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return _byte();

	return m_pInput_Device->Get_DIMouseState(eMouse);

}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseState)
{
	if (nullptr == m_pInput_Device)
		return _long();

	return m_pInput_Device->Get_DIMouseMove(eMouseState);

}

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Pressing(byKeyID);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Down(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Up(byKeyID);
}

_bool CGameInstance::Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Pressing(eMouse);
}

_bool CGameInstance::Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Down(eMouse);
}

_bool CGameInstance::Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Up(eMouse);
}

void CGameInstance::Release_Manager()
{
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);

	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Release_Manager();
	CGameInstance::GetInstance()->DestroyInstance();
}

