#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CMonster::CMonster(const CMonster& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)	// 복사 생성할때 원본이 갖고있는 멤버 변수가 있다면 직접 전해줘야한다. 
												// 이거때문에 레벨을 계속 못받아서 Shader컴포넌트를 만들지 못하고 터짐.
{
}

HRESULT CMonster::Initialize_Prototype(LEVEL eLevel)
{
	// 레벨 구분
	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 여기서 원하는 값을 세팅 하거나 */
	//MONSTER_DESC* pDesc = (MONSTER_DESC*)pArg;
	//
	//m_pTransformCom->Set_State(vPos)

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float4 vPosition(0.f, 0.f, 0.f, 0.f);

	if (nullptr != pArg) // 받아온 녀석이 있다면 형변환 시켜서 사용하자.
		vPosition = *(_float4*)pArg;

	// 이 녀석의 TransformCom을 이용해 위치를 지정해주자.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{



}

void CMonster::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta, true); // 뼈들이 갖고있는 애니메이션(Transform)에 맞게 모델을 움직인다.
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CMonster::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i); // 뼈 매트릭스 추가

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

// 이 클래스가 사용할 컴포넌트들을 준비하자.
HRESULT CMonster::Ready_Components()
{// 셰이더 관련 함수와 모델만 추가했다.
	///* For.Com_Shader */	// 셰이더 컴포넌트 추가 
	//if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"), // 에러 복사생성자에 멤버변수를 받지 않아서 레벨이 달랐음
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	// 이제 애니메이션 모델로 생성하기 위해, 위 셰이더 Model 컴포넌트를 셰이더 AnimModel 컴포넌트로 바꿔주자!
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */		// 모델 컴포넌트 추가
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

// 셰이더에 필요한 정보들을 넘겨주자.
HRESULT CMonster::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;
	return S_OK;
}

_bool CMonster::PickingGround(_float4* pOut)
{
	if (m_pModelCom == nullptr)
		return false;

	// 마우스의 월드 스페이스를 받는다.
	RAY MouseRayInWorldSpace = m_pGameInstance->Get_MouseRay_World(g_iWinSizeX, g_iWinSizeY);
	
	// 받은 마우스 월드 스페이스와 이 녀석의 월드 스페이스를 넘겨주고, 픽킹이 됐다면 픽킹된 값을 받아온다.
	if (m_pModelCom->Compute_MousePos_float4(MouseRayInWorldSpace, m_pTransformCom->Get_WorldMatrix(), pOut)) // 값 잘 받았음))
		return true;

	return false;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CMonster* pInstance = new CMonster(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	/* 컴포넌트들을 다 사용했으면 지워주자. */
	Safe_Release(m_pModelCom);	// 모델
	Safe_Release(m_pShaderCom);	// 셰이더
}

