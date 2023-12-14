#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

/* Rect를 만들었을 때랑 거의 흡사하다. */
// (CVIBuffer_Mesh)
class CMesh final : public CVIBuffer
{
protected:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:	// 세팅
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones);	// 모델의 초기 상태 (정면을 바라보게)를 위해 PivotMatrix값을 인자로 받아온다.
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones);

private:	// 
	char				m_szName[MAX_PATH];
	_uint				m_iMaterialIndex = { 0 };

	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;

	vector<_float4x4>	m_OffsetMatrices;

	vector<_float3>		m_MeshVertex;
	vector<_uint3>		m_MeshIndex;
private:
	HRESULT Ready_Vertices_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_Vertices_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);

public:
	_bool Compute_MousePos_float4(RAY _Ray, _matrix _WorldMatrix, _float4* pOut);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END