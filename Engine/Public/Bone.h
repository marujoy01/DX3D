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

public: // 뼈에 선형보간된 행열값을 적용시키기 위해 Set함수를 만들자.
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

	/* 이 뼈만의 상태행렬 */
	_float4x4			m_TransformationMatrix;

	/* 이 뼈만의 상태행렬 * 부모뼈의 m_CombindTransformationMatrix */
	_float4x4			m_CombinedTransformationMatrix;

public:
	static CBone* Create(aiNode* pAINode, _int iParentIndex);
	CBone* Clone(); // 클론 생성 (CBone 클래스는 멤버 변수로 포인터형을 사용하지 않기 때문에, 디폴트 복사생성자로 생성한다.)
	virtual void Free() override;
};

END