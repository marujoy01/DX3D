#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

}

// 2. 생성할때 넘겨받은 strNavigationFilePath를 파일 생성할 때 넣어주기
HRESULT CNavigation::Initialize_Prototype(const wstring& strNavigationFilePath)
{
	HANDLE		hFile = CreateFile(strNavigationFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = { 0 };

	while (true)
	{
		_float3		vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CNavigation::Render()
{
#ifdef _DEBUG
	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Render(m_pShader);
	}
#endif // _DEBUG

	return S_OK;
}

// 1. strNavigationFilePath를 네비게이션 생성할 때 받기
CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strNavigationFilePath))) // 받은 strNavigationFilePath 원본 이니셜라이즈 할때 넘겨주기
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
