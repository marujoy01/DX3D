#pragma once

#include "Component.h"

BEGIN(Engine)

// 컴포넌트
class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END }; // 모델의 타입

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const
	{
		return m_iNumMeshes;
	}
	void Set_AnimChange(_uint iChangeAnim)
	{
		m_iCurrentAnimIndex = iChangeAnim;
	}

public:
	virtual HRESULT Initialize_Prototype(const string & strModelFilePath, _fmatrix PivotMatrix, TYPE eType);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_float fTimeDelta, _bool isLoop);

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char * pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char * pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

public:
	_bool Compute_MousePos_float4(RAY _Ray, _matrix _WorldMatrix, _float4 * pOut);

private:
	const aiScene* m_pAIScene = { nullptr };	// 모델의 정보를 다 담고있는 녀석 (이녀석이 제대로 채워져있으면 모델로드가 잘된 것.)
	Assimp::Importer		m_Importer;	// 이녀석을 통해 함수를 호출하여 m_pAIScene을 채워준다.

private:
	_float4x4				m_PivotMatrix; // 피벗 매트릭스
	TYPE					m_eModelType = { TYPE_END };

	_uint					m_iNumMeshes = { 0 };	// 메쉬의 개수
	vector<class CMesh*>	m_Meshes;				// 생성된 메쉬를 벡터타입 배열에 저장해놓는다.

	_uint					m_iNumMaterials = { 0 };
	vector<MATERIAL_DESC>	m_Materials;

	/* 내 모델의 전체 뼈들을 부모관계를 포함하여 저장한다. */
	vector<class CBone*>	m_Bones;

	/* 애니메이션 변수 */
	_uint							m_iNumAnimations = { 0 };	// 애니메이션 개수
	_uint							m_iCurrentAnimIndex = { 0 };// 현재 애니메이션
	vector<class CAnimation*>		m_Animations;				// 애니메이션 저장

	/*
		버텍스와 인덱스(모델 메쉬)를 여러개로 나누는 이유
		- 만약 자주 교체가 되어야하는 모델일 경우 -> ex : 플레이어의 투구, 갑옷, 무기 등등
		한곳에 다 만들어버리면 투구 교체를 위해 전부 삭제하고 다시 할당해야 하는 비용이 많이드는 작업이기 때문에 보통 나눠서 만들어져있다.
		ex)
		0 : 플레이어
		1 : 투구
		2 : 갑옷
		3 : 무기
	*/
public:
	typedef vector<class CBone*>	BONES; // 퍼블릭으로 둬야함

private:
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix); // 메쉬를 모아둔다.
	HRESULT Ready_Materials(const string & strModelFilePath); // 머테리얼 준비
	HRESULT Ready_Bones(aiNode * pAINode, _int iParentIndex); // 뼈 준비
	HRESULT Ready_Animations();	// 애니메이션 준비

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const string & strModelFilePath, _fmatrix PivotMatrix);	// 생성
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;	// 삭제
};

END