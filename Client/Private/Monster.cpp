#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CMonster::CMonster(const CMonster& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)	// ���� �����Ҷ� ������ �����ִ� ��� ������ �ִٸ� ���� ��������Ѵ�. 
												// �̰Ŷ����� ������ ��� ���޾Ƽ� Shader������Ʈ�� ������ ���ϰ� ����.
{
}

HRESULT CMonster::Initialize_Prototype(LEVEL eLevel)
{
	// ���� ����
	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* ���⼭ ���ϴ� ���� ���� �ϰų� */
	//MONSTER_DESC* pDesc = (MONSTER_DESC*)pArg;
	//
	//m_pTransformCom->Set_State(vPos)

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float4 vPosition(0.f, 0.f, 0.f, 0.f);

	if (nullptr != pArg) // �޾ƿ� �༮�� �ִٸ� ����ȯ ���Ѽ� �������.
		vPosition = *(_float4*)pArg;

	// �� �༮�� TransformCom�� �̿��� ��ġ�� ����������.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{



}

void CMonster::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta, true); // ������ �����ִ� �ִϸ��̼�(Transform)�� �°� ���� �����δ�.
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
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i); // �� ��Ʈ���� �߰�

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

// �� Ŭ������ ����� ������Ʈ���� �غ�����.
HRESULT CMonster::Ready_Components()
{// ���̴� ���� �Լ��� �𵨸� �߰��ߴ�.
	///* For.Com_Shader */	// ���̴� ������Ʈ �߰� 
	//if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"), // ���� ��������ڿ� ��������� ���� �ʾƼ� ������ �޶���
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	// ���� �ִϸ��̼� �𵨷� �����ϱ� ����, �� ���̴� Model ������Ʈ�� ���̴� AnimModel ������Ʈ�� �ٲ�����!
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */		// �� ������Ʈ �߰�
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

// ���̴��� �ʿ��� �������� �Ѱ�����.
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

	// ���콺�� ���� �����̽��� �޴´�.
	RAY MouseRayInWorldSpace = m_pGameInstance->Get_MouseRay_World(g_iWinSizeX, g_iWinSizeY);
	
	// ���� ���콺 ���� �����̽��� �� �༮�� ���� �����̽��� �Ѱ��ְ�, ��ŷ�� �ƴٸ� ��ŷ�� ���� �޾ƿ´�.
	if (m_pModelCom->Compute_MousePos_float4(MouseRayInWorldSpace, m_pTransformCom->Get_WorldMatrix(), pOut)) // �� �� �޾���))
		return true;

	return false;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CMonster* pInstance = new CMonster(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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

	/* ������Ʈ���� �� ��������� ��������. */
	Safe_Release(m_pModelCom);	// ��
	Safe_Release(m_pShaderCom);	// ���̴�
}

