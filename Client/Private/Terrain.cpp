#include "stdafx.h"
#include "Terrain.h"
#include "GameInstance.h"


CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice,pContext)
{
	
}

CTerrain::CTerrain(const CTerrain& rhs)
	:CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)
{
}

HRESULT CTerrain::Initialize_Prototype(LEVEL eLevel)
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{

	if(FAILED(__super::Initialize(pArg))) 
		return E_FAIL;


	if(FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Priority_Tick(_float fTimeDelta)
{
	int i = 0;
}

void CTerrain::Tick(_float fTimeDelta)
{
	
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CTerrain::Render()
{
	//TODO 셰이더에게 행렬을 던져주는 행위는 반드시 셰이더의 비긴함수를 호출하기 이전에 해야한다.
	//! 그 이유는, 셰이더의 비긴함수 내에서 pPass->Apply(0, m_pContext); 코드를 수행한다.
	//! Apply 호출 후에 행렬을 던져줘도 에러는 나지 않지만, 안정성이 떨어진다.
	//! Apply 호출 후에 행렬을 던져주면, 어떤 때에는 정상적으로 수행되고, 어떤 때에는 값이 제대로 안 넘어가는 경우가 있다.
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! 이 셰이더에 0번째 패스로 그릴거야.
	m_pShaderCom->Begin(0);

	//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
	m_pVIBufferCom->Bind_VIBuffers();

	//! 바인딩된 정점, 인덱스를 그려
	m_pVIBufferCom->Render();
	
	m_pNavigationCom->Render();

	return S_OK;
}

HRESULT CTerrain::Ready_Components()
{
	/* For.Com_Shader_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	//! For.Com_Shader
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Mask */ // 지형을 섞기위한 Mask // 에러 : 툴 레벨로 ID를 주지않아서 터레인 생성에 실패했었고, 제대로 동작이 안되니, Imgui shutdown에 들어가 터졌음
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_MASK]))))
		return E_FAIL;

	//if (m_eCurrentLevelID != LEVEL_TOOL)
	{
		/* For.Com_Brush */ // 브러쉬 컴포넌트 추가
		if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain_Brush"),
			TEXT("Com_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_BRUSH]))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{ // 셰이더에 값 던져주기
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResourceArray(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	// 마스크
	if (FAILED(m_pTextureCom[TYPE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	// 브러쉬
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))	// 수정
	//	return E_FAIL;
	
	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	/* 원형 객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	/* 사본 객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();


	Safe_Release(m_pNavigationCom); // 네비게이션도 지워주자.

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	//Safe_Release(m_pTextureCom);
	// 배열 형식, ->Release 에러 : 텍스처 컴포넌트를 배열 형식으로 바꿨기 때문에, 하나하나 삭제 시켜주도록 바꿔줘야한다.
	for (size_t i = 0; i < TYPE_END; i++)
	{
		Safe_Release(m_pTextureCom[i]);
	}
}
