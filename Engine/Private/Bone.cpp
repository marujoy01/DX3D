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
	return new CBone(*this); // �߰� ������ �ްų� �����Ұ͵�, ���� �����ڰ� ���� �ִ°͵� �ƴϴ� ���� �Ҵ��Ͽ� �����ϴ°͸� ������. (this�� ������ �����Ͽ� ����)
}


void CBone::Free()
{
}