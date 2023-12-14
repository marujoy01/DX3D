#pragma once

#include "Model.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const
	{
		return m_szName;
	}

	_matrix Get_CombinedTransformationMatrix() const
	{
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

public: // ���� ���������� �࿭���� �����Ű�� ���� Set�Լ��� ������.
	void Set_TransformationMatrix(_fmatrix TransformationMatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(aiNode* pAINode, _int iParentIndex);
	void Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix);

private:
	_char				m_szName[MAX_PATH] = "";
	_int				m_iParentIndex = { 0 };

	/* �� ������ ������� */
	_float4x4			m_TransformationMatrix;

	/* �� ������ ������� * �θ���� m_CombindTransformationMatrix */
	_float4x4			m_CombinedTransformationMatrix;

public:
	static CBone* Create(aiNode* pAINode, _int iParentIndex);
	CBone* Clone(); // Ŭ�� ���� (CBone Ŭ������ ��� ������ ���������� ������� �ʱ� ������, ����Ʈ ��������ڷ� �����Ѵ�.)
	virtual void Free() override;
};

END