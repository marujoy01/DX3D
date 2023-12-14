#include "..\Public\Cell.h"
#include "Shader.h"
#include "VIBuffer_Cell.h"

#ifdef _DEBUG
#include "GameInstance.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
#ifdef _DEBUG
	, m_pGameInstance(CGameInstance::GetInstance())// �����ν��Ͻ� ����� ���� �غ�������
#endif
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

#ifdef _DEBUG
	Safe_AddRef(m_pGameInstance);  // �����ν��Ͻ��� ���������� Refī��Ʈ�� �÷�����. �÷����� �����°͵� ��������.
#endif
}

HRESULT CCell::Initialize(const _float3* pPoints)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3));

#ifdef _DEBUG


	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCell::Render(class CShader* pShader)
{
	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	pShader->Begin(0);

	m_pVIBuffer->Bind_VIBuffers();

	m_pVIBuffer->Render();

	return S_OK;
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pPoints)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CCell::Free()
{
#ifdef _DEBUG	
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pGameInstance); // �����
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
