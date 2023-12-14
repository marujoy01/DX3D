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

// 메쉬 세팅
HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones) // 최종적으로 여기에 뼈 세팅정보를 받기 위해 전달하고 받고를 반복하였음
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iNumVertexBuffers = 1;	// 아직은 한개만 사용하니 1.
	m_iNumVertices = pAIMesh->mNumVertices;	// pAIMesh가 갖고있는 버틱스(정점) 개수로 채운다.
	m_iStride = sizeof(VTXMESH);	// 내가 만든 구조체(Elements)로 설정 (Engine_Struct) ?

	m_iNumPrimitive = pAIMesh->mNumFaces;		// pAIMesh의 삼각형 개수
	m_iNumIndices = pAIMesh->mNumFaces * 3;	// pAIMesh의 삼각형 개수 * 3;
	m_iIndexStride = 4;	// 어지간하면 값이 초과될것이니 4로 지정
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER 버텍스 버퍼 세팅

	// 함수로 대체
	HRESULT		hr = CModel::TYPE_NONANIM == eModelType ? Ready_Vertices_NonAnim(pAIMesh, PivotMatrix) : Ready_Vertices_Anim(pAIMesh, Bones); // Bones까지 넘겨주기

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER 인덱스 버퍼 세팅
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	// 인덱스를 증가시킬 m_iNumIndices // 에러 : _short를 _uint로 바꿔줘야한다. 모델 대부분이 값을 넘어갈거기 때문.
	_uint* pIndices = new _uint[m_iNumIndices];

	_uint		iNumIndices = { 0 };

	// error : 피오나 모델 부를때 모닝스타에서 터지는거 여기서 모델 인덱스로 안잡아줘서 그랬음
	// 인덱스를 pAIMesh->mFaces[].mIndices[]에 있는 인덱스로 넣어주자.
	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		// 정점을 이루고있는 인덱스이다.
		// pAIMesh(이 메쉬의)->mFaces(삼각형개수[몇번째삼각형?0]).mIndices[정점의0번째(X)];
		// pAIMesh(이 메쉬의)->mFaces(삼각형개수[몇번째삼각형?0]).mIndices[정점의1번째(Y)];
		// pAIMesh(이 메쉬의)->mFaces(삼각형개수[몇번째삼각형?0]).mIndices[정점의2번째(Z)];
		// ex : 고블린 메쉬를 이루고있는 삼각형[0]번째.0번째 삼각형을 이루고있는 정점 0[X], 1[Y], 3[Z]
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];

		// 이 메쉬를 피킹하기 위한 인덱스를 추가해주자.
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

		m_MeshVertex.push_back(pVertices[i].vPosition); // 피킹을 위한 이 메쉬의 정점 위치를 저장해두자.

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}


	m_SubResourceData.pSysMem = pVertices;

	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
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

		m_MeshVertex.push_back(pVertices[i].vPosition); // 피킹을 위한 이 메쉬의 정점 위치를 저장해두자.

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

	}

	m_iNumBones = pAIMesh->mNumBones;

	/* 이 메시에게 영향을 주는 뼈을 순회하면서 각각의 뼈가 어떤 정점들에게 영향을 주는지 파악한다.*/
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

		// 가중치
		/* 이 뼈는 몇개의 정점에게 영향을 주는가?! */
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			/* pAIBone->mWeights[j].mVertexId : 이 뼈가 영향을 주는 j번째 정점의 인덱스 */
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

	// 메쉬 버텍스 정보

	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	/* 뼈 */

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

	// 마우스 광선 위치(시작점), 방향
	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&_Ray.vOrigin), WorldMatrix);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&_Ray.vDirection), WorldMatrix));

	_float		fDist;	// 만약 충돌했다면 거리를 반환 받아야 해서 만들어뒀다.

	_int		iMeshIndex = m_MeshIndex.size(); // 삼각형 개수 (삼각형의 개수만큼 순회해서 피킹된 녀석을 찾을거야)

	for (_uint i = 0; i < iMeshIndex; i++)
	{
		//_uint		iIndices = m_BoneIndices[i];
		_vector		vPickedPos;


		// 삼각형 인덱스의 정점 위치 3개를 받는다.
		//		m_MeshVertex(정점) 이녀석의 어떤 정점을 가져와야해? -> [[i번째 삼각형의].x번째정점]
		_vector	vVec0 = XMLoadFloat3(&m_MeshVertex[m_MeshIndex[i].ix]); // 0
		_vector	vVec1 = XMLoadFloat3(&m_MeshVertex[m_MeshIndex[i].iy]); // 1
		_vector	vVec2 = XMLoadFloat3(&m_MeshVertex[m_MeshIndex[i].iz]); // 2

		/*
		ex : m_MeshVertex라는 정점 정보들을 갖고있는 녀석의
				i(0)번째 삼각형의
				0번 정점,
				1번 정점,
				2번 정점
				의 위치들을
				vVec에 담아.
		*/

		// 담아온 정점 위치 3개와 마우스를 비교하러 보내줘
		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vVec0, vVec1, vVec2, fDist))
		{
			// 비교했더니 이 메쉬의 특정 정점과 마우스의 레이저가 충돌했어
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist; // 충돌된 위치를 계산해서 담아

			XMStoreFloat4(pOut, vPickedPos); // 충돌된 위치를 pOut에 담아서 돌려보내줘

			return true; // 값도 보내고, 충돌도 성공했다고 알려
		}
	}

	return false; // 값 없이 충돌 실패했다고 알려
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones) // 뼈 세팅정보를 생성할 때 넘겨받음
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	// pAIMesh에 정보가 담겨있어 확인가능
	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, PivotMatrix, Bones))) // 넘겨받은 뼈 세팅 정보 전달
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

// 클론은 나중에 더 디테일한 수업으로..
CComponent* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	__super::Free();
}