#pragma once

#include "Renderer.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Component_Manager.h"
#include "Model.h"

/* 클라이언트에서 엔진의 기능을 사용하기위해 반드시 거쳐야하는 객체. */

/* DX11을 통해 필요한 객체를 생성하고 렌더링하는 작업을 수행한다. */
/* 지금 생성한 레벨(씬)을 보관하고 필요에따라 업데이트 수행. */
/* 내가 생성한 객체들을 보관하고 업데이트하고 렌더한다. */
/* 내가 생성한 여러 컴포넌트들의 원형을 보관하고 복제하여 가져온다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	/* 엔진라이브러리를 사용하기위한 준비를 모두 거친다. */
	HRESULT Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT  Render_Engine();
	void Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */		
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();
	HRESULT UseFullScreen(_bool bMode);
	HRESULT Resize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	GRAPHIC_DESC Get_GraphicDesc() { return m_tGraphicDesc; }

public: /* For.Timer_Manager */
	HRESULT	Add_Timer(const wstring& strTimeTag);
	_float Compute_TimeDelta(const wstring& strTimeTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iCurrentLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr, CGameObject** ppOut = nullptr);
	map<const wstring, class CGameObject*>* Get_GameObject();

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void* pArg);

public: /* For.Renderer */
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, class CGameObject* pGameObject);
	
public: /* For.PipeLine */
	void		Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void		Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix);

	_matrix		Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eState);
	_float4x4	Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eState);

	_matrix		Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE eState);
	_float4x4	Get_TransformFloat4x4Inverse(CPipeLine::D3DTRANSFORMSTATE eState);

	// 레이
	RAY			Get_MouseRay_World(const _uint & In_ViewPortWidth, const _uint & In_ViewPortHeight);

	_float4		Get_CamPosition();

public: /* For.Input_Device */
	void	Set_hWnd_hInst(HINSTANCE hInst, HWND hWnd);
	
	_byte   Get_DIKeyState(_ubyte byKeyID);
	_byte   Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouse);
	_long   Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseState);
	_bool	Key_Pressing(_ubyte byKeyID);
	_bool	Key_Down(_ubyte byKeyID);
	_bool	Key_Up(_ubyte byKeyID);
	_bool	Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouse);
	_bool	Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouse);
	_bool	Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouse);

private:
	GRAPHIC_DESC					m_tGraphicDesc = {};
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };

private:
	//#게임인스턴스핸들
	HWND	  m_hWnd = { nullptr };
	HINSTANCE m_hInst = { nullptr };


public:
	void Release_Manager();
	static void Release_Engine();
};

END