#include "..\Public\Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode* pAINode, _int iParentIndex)
{
	m_iParentIndex = iParentIndex; // -1

	strcpy_s(m_szName, pAINode->mName.data); // RootNode

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4)); // 1 0 0

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix))); // 1 0 0

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity()); // 1 0 0

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix)
{
	if (-1 == m_iParentIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PivotMatrix);
	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));
	}

}

CBone* CBone::Create(aiNode* pAINode, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this); // 추가 적으로 받거나 세팅할것도, 복사 생성자가 따로 있는것도 아니니 새로 할당하여 생성하는것만 해주자. (this로 본인을 복사하여 생성)
}


void CBone::Free()
{
}