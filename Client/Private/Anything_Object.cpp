#include "stdafx.h"
#include "..\Public\Anything_Object.h"

#include "GameInstance.h"

CAnything_Object::CAnything_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CAnything_Object::CAnything_Object(const CAnything_Object& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)	// ���� �����Ҷ� ������ �����ִ� ��� ������ �ִٸ� ���� ��������Ѵ�. 
												// �̰Ŷ����� ������ ��� ���޾Ƽ� Shader������Ʈ�� ������ ���ϰ� ����.
{
}

HRESULT CAnything_Object::Initialize_Prototype(LEVEL eLevel)
{
	// ���� ����
	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CAnything_Object::Initialize(void* pArg) // ���� �ȹ޾ƿ���
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* ���⼭ ���ϴ� ���� ���� �ϰų� */
	//MONSTER_DESC* pDesc = (MONSTER_DESC*)pArg;
	//
	//m_pTransformCom->Set_State(vPos)

	ANYTHINGOBJ* pAnyInfo = (ANYTHINGOBJ*)pArg;

	// ��ġ ��
	m_vPosition = pAnyInfo->vPosition;

	// �� ������Ʈ �ױ�
	m_wModelTag = pAnyInfo->cModelTag;
	m_wShaderTag = pAnyInfo->cShaderTag;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimChange(0);

	// �� �༮�� TransformCom�� �̿��� ��ġ�� ����������.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);

	return S_OK;
}

void CAnything_Object::Priority_Tick(_float fTimeDelta)
{



}

void CAnything_Object::Tick(_float fTimeDelta)
{
	// �����̽��� �����µ�
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		if (m_bAnimLoop) // �����־�����
		{
			m_bAnimLoop = false; // ����
		}
		else // �����־�����
		{
			m_bAnimLoop = true; // �Ѱ�
		}
	}

	if (m_pGameInstance->Key_Down(DIK_LEFT))
	{
		m_pModelCom->Set_AnimChange(-1);
	}
	if (m_pGameInstance->Key_Down(DIK_RIGHT))
	{
		m_pModelCom->Set_AnimChange(1);
	}


	// �ִϸ��̼��� �ݺ� ������� ���� �����ϱ� ���� bool���� ��������.
	m_pModelCom->Play_Animation(fTimeDelta, true); // ������ �����ִ� �ִϸ��̼�(Transform)�� �°� ���� �����δ�.
}

void CAnything_Object::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CAnything_Object::Render()
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
HRESULT CAnything_Object::Ready_Components()
{// ���̴� ���� �Լ��� �𵨸� �߰��ߴ�.
	///* For.Com_Shader */	// ���̴� ������Ʈ �߰� 
	//if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"), // ���� ��������ڿ� ��������� ���� �ʾƼ� ������ �޶���
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	// ���� �ִϸ��̼� �𵨷� �����ϱ� ����, �� ���̴� Model ������Ʈ�� ���̴� AnimModel ������Ʈ�� �ٲ�����!
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, m_wShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */		// �� ������Ʈ �߰�
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, m_wModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

// ���̴��� �ʿ��� �������� �Ѱ�����.
HRESULT CAnything_Object::Bind_ShaderResources()
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

_bool CAnything_Object::PickingObject(_float4* pOut)
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

void CAnything_Object::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	// ������Ʈ�� ��ȸ�ϸ� Ʈ������ ������Ʈ�� ã����.
	auto iter = Out_Json["Component"].find("Transform");

}

void CAnything_Object::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

	_float4x4 WorldMatrix;
	// ���� �� �ʱ�ȭ
	ZeroMemory(&WorldMatrix, sizeof(_float4x4));

	// �� �ҷ�����
	CJson_Utility::Load_JsonFloat4x4(In_Json["Component"]["Transform"], WorldMatrix);

	// �ҷ��� ���� ���� �־�����.
	_float4 fWorld = { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, 1.f };

	// �� �༮�� ���� ���� ������ ����������.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fWorld);
}

CAnything_Object* CAnything_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CAnything_Object* pInstance = new CAnything_Object(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CAnything_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAnything_Object::Clone(void* pArg)
{
	CAnything_Object* pInstance = new CAnything_Object(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAnything_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnything_Object::Free()
{
	__super::Free();

	/* ������Ʈ���� �� ��������� ��������. */
	Safe_Release(m_pModelCom);	// ��
	Safe_Release(m_pShaderCom);	// ���̴�
}

