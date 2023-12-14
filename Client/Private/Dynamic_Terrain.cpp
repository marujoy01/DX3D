#include "stdafx.h"
#include "Dynamic_Terrain.h"
#include "GameInstance.h"

CDynamic_Terrain::CDynamic_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{

}

CDynamic_Terrain::CDynamic_Terrain(const CDynamic_Terrain& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)
{
}

HRESULT CDynamic_Terrain::Initialize_Prototype(LEVEL eLevel)
{
	//TODO ������ü�� �ʱ�ȭ������ �����Ѵ�.
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/
	
	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CDynamic_Terrain::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* ���� ���� ���� */
	/* 
		1. ���� �����͸� ĳ������ ���� DINFO����ü�� �ٲ㼭 ����ְ�, �� ����ü�� ���� �� �ٸ� �׸��� ��Ƽ� �Լ��� �Ѱ�����.
				* �� ����� ��ġ���ʰ� �׳� �ּҷ� �Ѱ����� ��, ���� ����� �ȳѾ�� *
	*/

	// ����ü ĳ����
	DINFO* pInfo = (DINFO*)pArg; 
	
	// ĳ���õ� ����ü�� �� ���
	DINFO Info = { pInfo->fX, pInfo->fY, pInfo->fZ };

	// �ּҰ� �ƴ�, ���� ��� ����ü �Ѱ��ֱ�
	ReceiveInfo(Info);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CDynamic_Terrain::Priority_Tick(_float fTimeDelta)
{
}

void CDynamic_Terrain::Tick(_float fTimeDelta)
{
	// ���콺 ������
	if (PickingGround(&pOut))
	{
		m_vMousePos = XMLoadFloat4(&pOut);
	}
}

void CDynamic_Terrain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CDynamic_Terrain::Render()
{
	//TODO ���̴����� ����� �����ִ� ������ �ݵ�� ���̴��� ����Լ��� ȣ���ϱ� ������ �ؾ��Ѵ�.
	//! �� ������, ���̴��� ����Լ� ������ pPass->Apply(0, m_pContext); �ڵ带 �����Ѵ�.
	//! Apply ȣ�� �Ŀ� ����� �����൵ ������ ���� ������, �������� ��������.
	//! Apply ȣ�� �Ŀ� ����� �����ָ�, � ������ ���������� ����ǰ�, � ������ ���� ����� �� �Ѿ�� ��찡 �ִ�.
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (m_pVIBufferCom != nullptr) // ���� �����Ҷ� �����ϰ� ��������� ������ Ÿ�� �����⶧����, ����ó�� �ص���.
	{
		//! ���� �׸����� �ϴ� ����, �ε��� ���۸� ��ġ�� ���ε���
		m_pVIBufferCom->Bind_VIBuffers(); // ���� : �귯���� �߰��ϰ� ������ ���⼭ ���� �߻� -> �귯�� �ʿ������. �Ⱦ���

		//! �� ���̴��� 0��° �н��� �׸��ž�.
		m_pShaderCom->Begin(0);

		//! ���ε��� ����, �ε����� �׷�
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

HRESULT CDynamic_Terrain::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer_Dynamic
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_VIBuffer_Dynamic_Terrain"),	// �������� ����
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &m_tDynamicInfo)))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Mask */ // ������ �������� Mask // ���� : �� ������ ID�� �����ʾƼ� �ͷ��� ������ �����߾���, ����� ������ �ȵǴ�, Imgui shutdown�� �� ������
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_MASK]))))
		return E_FAIL;

	/* For.Com_Brush */ // �귯�� ������Ʈ �߰�
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain_Brush"),
		TEXT("Com_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_BRUSH]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamic_Terrain::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;	
	
	if (FAILED(m_pShaderCom->Bind_Vector("g_vBrushPos", &m_vMousePos)))
		return E_FAIL;

	// ��ǻ��
	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResourceArray(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	// ����ũ
	if (FAILED(m_pTextureCom[TYPE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	// �귯��
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CDynamic_Terrain::ReceiveInfo(DINFO pInfo)
{
	m_tDynamicInfo = { pInfo.fX, pInfo.fY, pInfo.fZ };
}

void CDynamic_Terrain::PickingGround(EDIT_MODE _eMode)
{
	if (m_pVIBufferCom == nullptr)
		return;

	//if (m_pGameInstance->Mouse_Down(CInput_Device::DIM_LB) == false)
	//	return;

	RAY MouseRayInWorldSpace = m_pGameInstance->Get_MouseRay_World(g_iWinSizeX, g_iWinSizeY);

	if (m_pVIBufferCom != nullptr)
	{
		_float3		Out = _float3(0.f, 0.f, 0.f);

		if (m_pVIBufferCom->Compute_MousePos(MouseRayInWorldSpace, m_pTransformCom->Get_WorldMatrix(), &Out)) // �� �� �޾���))
		{
			m_pVIBufferCom->Tick(XMLoadFloat3(&Out), m_fBufferDrawRadious, m_fBufferPower, (_uint)_eMode);
		}
	}
}

_bool CDynamic_Terrain::PickingGround(_float4* pOut)
{
	if (m_pVIBufferCom == nullptr)
		return false;

	// ���콺�� ���� �����̽��� �޴´�.
	RAY MouseRayInWorldSpace = m_pGameInstance->Get_MouseRay_World(g_iWinSizeX, g_iWinSizeY);

	// ���� ���콺 ���� �����̽��� �� �༮�� ���� �����̽��� �Ѱ��ְ�, ��ŷ�� �ƴٸ� ��ŷ�� ���� �޾ƿ´�.
	if (m_pVIBufferCom->Compute_MousePos_float4(MouseRayInWorldSpace, m_pTransformCom->Get_WorldMatrix(), pOut)) // �� �� �޾���))
		return true;

	return false;
}

void CDynamic_Terrain::Delete_Component(const wstring& strComTag)
{
	__super::Delete_Component(strComTag);

	// ! ������ �ȵɶ� !
	// Delete_Component������ ����� �༮�� Safe_Release�� ���۷��� ī��Ʈ�� ���̱⸸ �Ѱ��̶�,
	// ����� Ÿ���� Ȯ���ϰ� ���⼭ ��������� ���� ��������Ѵ�.

	Safe_Release(m_pVIBufferCom);

}

CDynamic_Terrain* CDynamic_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CDynamic_Terrain* pInstance = new CDynamic_Terrain(pDevice, pContext);

	/* ���� ��ü�� �ʱ�ȭ�Ѵ�. */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CDynamic_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDynamic_Terrain::Clone(void* pArg) // ���⼭ �纻�� ���鶧 ������ ������ ���� �޾��ش�.
{
	CDynamic_Terrain* pInstance = new CDynamic_Terrain(*this);

	/* �纻 ��ü�� �ʱ�ȭ�Ѵ�. */
	if (FAILED(pInstance->Initialize(pArg))) // �Ѱ���
	{
		MSG_BOX("Failed to Cloned : CDynamic_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDynamic_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	//Safe_Release(m_pTextureCom);
	// �迭 ����, ->Release ���� : �ؽ�ó ������Ʈ�� �迭 �������� �ٲ�� ������, �ϳ��ϳ� ���� �����ֵ��� �ٲ�����Ѵ�.
	for (size_t i = 0; i < TYPE_END; i++)
	{
		Safe_Release(m_pTextureCom[i]);
	}
}
