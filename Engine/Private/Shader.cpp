#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice,pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_InputLayouts(rhs.m_InputLayouts)
	, m_TechniqueDesc(rhs.m_TechniqueDesc)
{
	for (auto& pInputLayout : m_InputLayouts)
	{
		Safe_AddRef(pInputLayout); //! vector(m_InputLayouts)안의 InputLayout 컴객체를 포인터로 참조하고있다. 루프 돌면서 레퍼런스 올리자
	}

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint	iHlslFlag = 0;

	#ifdef _DEBUG
		iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#else
		iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1; //! 가장 최소한의 최적화를 사용하겠다.
	#endif
	
	
	//!strShaderFilePath 경로에 작성되어 있는 hlsl언어 번역 빌드하여 ID3DX11Effect라는 녀석을 만들자
	if(FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,iHlslFlag,0, m_pDevice,&m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0); //! 우리는 테크니크 한개니까 0으로하자
	if(nullptr == pTechnique)
		return E_FAIL;

	pTechnique->GetDesc(&m_TechniqueDesc);

	for (size_t i = 0; i < m_TechniqueDesc.Passes; i++)
	{
		ID3DX11EffectPass*	pPass = pTechnique->GetPassByIndex(i);
		
		D3DX11_PASS_DESC	PassDesc;
		pPass->GetDesc(&PassDesc);

		/* InputLayout : 내가 그리기위해 사용하는 정점의 입력정보.  */
	    /* dx11에서는 고정기능 렌더링파이프라인에 대한 기능이 삭제되었다. */
	    /* 우리가 직접 렌더링 파이프라인을 수행해야한다.(사용자정의 렌더링파이프라인) -> Shader */
	    /* 그래서 우리에겐 반드시 셰이더가 필요하다. */
	    /* 우리가 이 정점들을 그리기위해서는 셰이더가 필요하고, 이 셰이더는 반드시 내가 그릴려고하는 정점을 받아줄 수 있어야한다. */
	    /* 내가 그리려고하는 정점이 사용하려고하는 셰이더에 입력이 가능한지?에 대한 체크를 사전에 미리 처리하고.
	    가능하다면 dx11이 InputLayout이란 객체를 만들어준다. */
		ID3D11InputLayout*	pInputLayout = nullptr;

		if(FAILED(m_pDevice->CreateInputLayout(pElements,iNumElements,PassDesc.pIAInputSignature,PassDesc.IAInputSignatureSize,&pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if(iPassIndex >= m_TechniqueDesc.Passes)
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0); //! 테크니크 한개잖아.
	if(nullptr == pTechnique)
		return E_FAIL;

	ID3DX11EffectPass*	pPass = pTechnique->GetPassByIndex(iPassIndex);

	pPass->Apply(0, m_pContext); //! 패스는 0으로 고정, Apply는 장치에게 이 패스를 사용해서 그릴 것이라고 알림.

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);
	
	return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	//! 이 셰이더에 선언되어 있는 전역변수(인자값으로 들어온 이름과 같은)의 핸들을 얻어온다.
	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName);

	//! 만약, 셰이더파일내의 이름이 일치하는 전역변수가 없다면 리턴
	if(nullptr == pVariable)
		return E_FAIL;

	//! 행렬 타입 전역변수핸들의 행렬 가져오기
	ID3DX11EffectMatrixVariable*	pMatrixVariable = pVariable->AsMatrix();
	if(nullptr == pMatrixVariable)
		return E_FAIL;
	
	//! 행렬 타입 전역변수의 값을 인자값으로 들어온 행렬로 채워준다.
	//! 이로 인해, 우리는 클라이언트에서 셰이더 파일에게 행렬을 던져준 것.
	return pMatrixVariable->SetMatrix((_float*)pMatrix);

}

HRESULT CShader::Bind_Vector(const _char* pConstantName, const _vector* pVector)
{
	//! 이 셰이더에 선언되어 있는 전역변수(인자값으로 들어온 이름과 같은)의 핸들을 얻어온다.
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);

	//! 만약, 셰이더파일내의 이름이 일치하는 전역변수가 없다면 리턴
	if (nullptr == pVariable)
		return E_FAIL;

	//! 벡터 타입 전역변수핸들의 행렬 가져오기
	ID3DX11EffectVectorVariable* pMatrixVariable = pVariable->AsVector();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	//! 벡터 타입 전역변수의 값을 인자값으로 들어온 벡터로 채워준다.
	//! 이로 인해, 우리는 클라이언트에서 셰이더 파일에게 벡터를 던져준 것.
	return pMatrixVariable->SetFloatVector((_float*)pVector);

}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
	//! 이 셰이더에 선언되어 있는 전역변수(인자값으로 들어온 이름과 같은)의 핸들을 얻어온다.
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);

	//! 만약, 셰이더파일내의 이름이 일치하는 전역변수가 없다면 리턴
	if (nullptr == pVariable)
		return E_FAIL;

	//! 행렬 타입 전역변수핸들의 행렬 가져오기
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;
	
	//! 행렬 타입 전역변수의 값을 인자값으로 들어온 행렬로 채워준다.
	//! 이로 인해, 우리는 클라이언트에서 셰이더 파일에게 행렬을 던져준 것.
	return pMatrixVariable->SetMatrixArray((_float*)pMatrix,0,iNumMatrices);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	//! 이 셰이더에 선언되어 있는 전역변수(인자값으로 들어온 이름과 같은)의 핸들을 얻어온다.
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);

	//! 만약, 셰이더파일내의 이름이 일치하는 전역변수가 없다면 리턴
	if (nullptr == pVariable)
		return E_FAIL;

	//! 셰이더리소스 타입 전역변수핸들의 셰이더리소스 가져오기
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	//! 셰이더 리소스뷰 타입 전역변수의 값을 인자값으로 들어온 셰이더 리소스뷰로 채워준다.
	//! 이로 인해, 우리는 클라이언트에서 셰이더 파일에게 셰이더 리소스뷰를 던져준 것.
	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures)
{
	//! 이 셰이더에 선언되어 있는 전역변수(인자값으로 들어온 이름과 같은)의 핸들을 얻어온다.
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);

	//! 만약, 셰이더파일내의 이름이 일치하는 전역변수가 없다면 리턴
	if (nullptr == pVariable)
		return E_FAIL;

	//! 셰이더리소스 타입 전역변수핸들의 셰이더리소스 가져오기
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	//! 셰이더 리소스뷰 타입 전역변수의 값을 인자값으로 들어온 셰이더 리소스뷰로 채워준다.
	//! 이로 인해, 우리는 클라이언트에서 셰이더 파일에게 셰이더 리소스뷰를 던져준 것.
	return pSRVariable->SetResourceArray(ppSRV,0, iNumTextures);
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iSize)
{
	//	셰이더 파일에있는 변수의 핸들을 가져온다.
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	// 가져온 셰이더 파일의 변수에 원하는걸 세팅해준다.
	return pVariable->SetRawValue(pData, 0, iSize);
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
	{
		Safe_Release(pInputLayout);
	}

	Safe_Release(m_pEffect);
}
