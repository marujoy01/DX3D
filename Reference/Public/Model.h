#pragma once

#include "Component.h"

BEGIN(Engine)

// ������Ʈ
class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END }; // ���� Ÿ��

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
	const aiScene* m_pAIScene = { nullptr };	// ���� ������ �� ����ִ� �༮ (�̳༮�� ����� ä���������� �𵨷ε尡 �ߵ� ��.)
	Assimp::Importer		m_Importer;	// �̳༮�� ���� �Լ��� ȣ���Ͽ� m_pAIScene�� ä���ش�.

private:
	_float4x4				m_PivotMatrix; // �ǹ� ��Ʈ����
	TYPE					m_eModelType = { TYPE_END };

	_uint					m_iNumMeshes = { 0 };	// �޽��� ����
	vector<class CMesh*>	m_Meshes;				// ������ �޽��� ����Ÿ�� �迭�� �����س��´�.

	_uint					m_iNumMaterials = { 0 };
	vector<MATERIAL_DESC>	m_Materials;

	/* �� ���� ��ü ������ �θ���踦 �����Ͽ� �����Ѵ�. */
	vector<class CBone*>	m_Bones;

	/* �ִϸ��̼� ���� */
	_uint							m_iNumAnimations = { 0 };	// �ִϸ��̼� ����
	_uint							m_iCurrentAnimIndex = { 0 };// ���� �ִϸ��̼�
	vector<class CAnimation*>		m_Animations;				// �ִϸ��̼� ����

	/*
		���ؽ��� �ε���(�� �޽�)�� �������� ������ ����
		- ���� ���� ��ü�� �Ǿ���ϴ� ���� ��� -> ex : �÷��̾��� ����, ����, ���� ���
		�Ѱ��� �� ���������� ���� ��ü�� ���� ���� �����ϰ� �ٽ� �Ҵ��ؾ� �ϴ� ����� ���̵�� �۾��̱� ������ ���� ������ ��������ִ�.
		ex)
		0 : �÷��̾�
		1 : ����
		2 : ����
		3 : ����
	*/
public:
	typedef vector<class CBone*>	BONES; // �ۺ����� �־���

private:
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix); // �޽��� ��Ƶд�.
	HRESULT Ready_Materials(const string & strModelFilePath); // ���׸��� �غ�
	HRESULT Ready_Bones(aiNode * pAINode, _int iParentIndex); // �� �غ�
	HRESULT Ready_Animations();	// �ִϸ��̼� �غ�

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const string & strModelFilePath, _fmatrix PivotMatrix);	// ����
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;	// ����
};

END