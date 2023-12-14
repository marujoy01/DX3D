#include "Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroupID, CGameObject* pGameObject)
{
	if(nullptr == pGameObject || eGroupID >= RENDER_END) //! 인자값으로 들어온 객체가 nullptr 거나 들어온 enum값이 RENDERGROUP 열거체의 마지막을 나타내는 RENDER_END보다 크거나 같은값이 들어온 경우의 예외처리
		return E_FAIL;

	m_RenderObjects[eGroupID].push_back(pGameObject);
	
	Safe_AddRef(pGameObject); //! 오브젝트 매니저에서 저장하여 관리하는 게임오브젝트 객체를 공유하는 행위이기 때문에 레퍼런스 카운트를 증가시켜준다.

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if(FAILED(Render_Priority()))
		return E_FAIL;
	if(FAILED(Render_NonLight()))
		return E_FAIL;
	if(FAILED(Render_NonBlend()))
		return E_FAIL;
	if(FAILED(Render_Blend()))
		return E_FAIL;
	if(FAILED(Render_UI()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if(nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONBLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderer::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
