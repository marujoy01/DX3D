#include "..\Public\Mesh.h"
#include "Shader.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

// �޽� ����
HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones) // ���������� ���⿡ �� ���������� �ޱ� ���� �����ϰ� �ް� �ݺ��Ͽ���
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iNumVertexBuffers = 1;	// ������ �Ѱ��� ����ϴ� 1.
	m_iNumVertices = pAIMesh->mNumVertices;	// pAIMesh�� �����ִ� ��ƽ��(����) ������ ä���.
	m_iStride = sizeof(VTXMESH);	// ���� ���� ����ü(Elements)�� ���� (Engine_Struct) ?

	m_iNumPrimitive = pAIMesh->mNumFaces;		// pAIMesh�� �ﰢ�� ����
	m_iNumIndices = pAIMesh->mNumFaces * 3;	// pAIMesh�� �ﰢ�� ���� * 3;
	m_iIndexStride = 4;	// �������ϸ� ���� �ʰ��ɰ��̴� 4�� ����
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER ���ؽ� ���� ����

	// �Լ��� ��ü
	HRESULT		hr = CModel::TYPE_NONANIM == eModelType ? Ready_Vertices_NonAnim(pAIMesh, PivotMatrix) : Ready_Vertices_Anim(pAIMesh, Bones); // Bones���� �Ѱ��ֱ�

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER �ε��� ���� ����
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	// �ε����� ������ų m_iNumIndices // ���� : _short�� _uint�� �ٲ�����Ѵ�. �� ��κ��� ���� �Ѿ�ű� ����.
	_uint* pIndices = new _uint[m_iNumIndices];

	_uint		iNumIndices = { 0 };

	// error : �ǿ��� �� �θ��� ��׽�Ÿ���� �����°� ���⼭ �� �ε����� ������༭ �׷���
	// �ε����� pAIMesh->mFaces[].mIndices[]�� �ִ� �ε����� �־�����.
	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		// ������ �̷���ִ� �ε����̴�.
		// pAIMesh(�� �޽���)->mFaces(�ﰢ������[���°�ﰢ��?0]).mIndices[������0��°(X)];
		// pAIMesh(�� �޽���)->mFaces(�ﰢ������[���°�ﰢ��?0]).mIndices[������1��°(Y)];
		// pAIMesh(�� �޽���)->mFaces(�ﰢ������[���°�ﰢ��?0]).mIndices[������2��°(Z)];
		// ex : ��� �޽��� �̷���ִ� �ﰢ��[0]��°.0��° �ﰢ���� �̷���ִ� ���� 0[X], 1[Y], 3[Z]
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];

		// �� �޽��� ��ŷ�ϱ� ���� �ε����� �߰�������.
		m_MeshIndex.push_back({ pAIMesh->mFaces[i].mIndices[0],
								pAIMesh->mFaces[i].mIndices[1],
								pAIMesh->mFaces[i].mIndices[2] });
	}

	//pIndices[0] = 0;
	//pIndices[1] = 1;
	//pIndices[2] = 2;

	//pIndices[3] = 0;
	//pIndices[4] = 2;
	//pIndices[5] = 3;

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB))) // morningstar, cake
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones)
{
	_float4x4		BoneMatrices[256];

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);
}

HRESULT CMesh::Ready_Vertices_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		m_MeshVertex.push_back(pVertices[i].vPosition); // ��ŷ�� ���� �� �޽��� ���� ��ġ�� �����ص���.

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}


	m_SubResourceData.pSysMem = pVertices;

	/* pVertices�� �Ҵ��Ͽ� ä������ �������� ������ ID3D11Buffer�� �Ҵ��� ������ �����Ͽ� ä���ִ´�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones)
{
	m_iStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];

	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices); // ZeroMemory +

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));

		m_MeshVertex.push_back(pVertices[i].vPosition); // ��ŷ�� ���� �� �޽��� ���� ��ġ�� �����ص���.

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

	}

	m_iNumBones = pAIMesh->mNumBones;

	/* �� �޽ÿ��� ������ �ִ� ���� ��ȸ�ϸ鼭 ������ ���� � �����鿡�� ������ �ִ��� �ľ��Ѵ�.*/
	for (size_t i = 0; i < pAIMesh->mNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4		OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		_uint		iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
			{
				if (false == strcmp(pAIBone->mName.data, pBone->Get_Name()))
				{
					return true;
				}

				++iBoneIndex;

				return false;
			});

		if (iter == Bones.end())
			return E_FAIL;

		m_BoneIndices.push_back(iBoneIndex);

		// ����ġ
		/* �� ���� ��� �������� ������ �ִ°�?! */
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			/* pAIBone->mWeights[j].mVertexId : �� ���� ������ �ִ� j��° ������ �ε��� */
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	};

	m_SubResourceData.pSysMem = pVertices;

	// �޽� ���ؽ� ����

	/* pVertices�� �Ҵ��Ͽ� ä������ �������� ������ ID3D11Buffer�� �Ҵ��� ������ �����Ͽ� ä���ִ´�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	/* �� */

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint		iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
			{
				if (false == strcmp(m_szName, pBone->Get_Name()))
				{
					return true;
				}

				++iBoneIndex;

				return false;
			});

		if (iter == Bones.end())
			return E_FAIL;

		m_BoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	return S_OK;

}

// vector
_bool CMesh::Compute_MousePos_float4(RAY _Ray, _matrix _WorldMatrix, _float4* pOut)
{
	_matrix		WorldMatrix = XMMatrixInverse(nullptr, _WorldMatrix);
	_vector		vRayPos, vRayDir;

	// ���콺 ���� ��ġ(������), ����
	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&_Ray.vOrigin), WorldMatrix);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&_Ray.vDirection), WorldMatrix));

	_float		fDist;	// ���� �浹�ߴٸ� �Ÿ��� ��ȯ �޾ƾ� �ؼ� �����״�.

	_int		iMeshIndex = m_MeshIndex.size(); // �ﰢ�� ���� (�ﰢ���� ������ŭ ��ȸ�ؼ� ��ŷ�� �༮�� ã���ž�)

	for (_uint i = 0; i < iMeshIndex; i++)
	{
		//_uint		iIndices = m_BoneIndices[i];
		_vector		vPickedPos;


		// �ﰢ�� �ε����� ���� ��ġ 3���� �޴´�.
		//		m_MeshVertex(����) �̳༮�� � ������ �����;���? -> [[i��° �ﰢ����].x��°����]
		_vector	vVec0 = XMLoadFloat3(&m_MeshVertex[m_MeshIndex[i].ix]); // 0
		_vector	vVec1 = XMLoadFloat3(&m_MeshVertex[m_MeshIndex[i].iy]); // 1
		_vector	vVec2 = XMLoadFloat3(&m_MeshVertex[m_MeshIndex[i].iz]); // 2

		/*
		ex : m_MeshVertex��� ���� �������� �����ִ� �༮��
				i(0)��° �ﰢ����
				0�� ����,
				1�� ����,
				2�� ����
				�� ��ġ����
				vVec�� ���.
		*/

		// ��ƿ� ���� ��ġ 3���� ���콺�� ���Ϸ� ������
		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vVec0, vVec1, vVec2, fDist))
		{
			// ���ߴ��� �� �޽��� Ư�� ������ ���콺�� �������� �浹�߾�
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist; // �浹�� ��ġ�� ����ؼ� ���

			XMStoreFloat4(pOut, vPickedPos); // �浹�� ��ġ�� pOut�� ��Ƽ� ����������

			return true; // ���� ������, �浹�� �����ߴٰ� �˷�
		}
	}

	return false; // �� ���� �浹 �����ߴٰ� �˷�
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones) // �� ���������� ������ �� �Ѱܹ���
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	// pAIMesh�� ������ ����־� Ȯ�ΰ���
	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, PivotMatrix, Bones))) // �Ѱܹ��� �� ���� ���� ����
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

// Ŭ���� ���߿� �� �������� ��������..
CComponent* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	__super::Free();
}