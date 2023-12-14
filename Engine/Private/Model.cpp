#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)	// 복사 생성자. (클론을 만들때 원본 값을, 클론 멤버변수에 복사해주자.)
	: CComponent(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_eModelType(rhs.m_eModelType)
	, m_iNumMeshes(rhs.m_iNumMeshes)	// 메쉬 개수 멤버변수 복사
	, m_Meshes(rhs.m_Meshes)			// 메쉬모음 멤버변수 복사 (vector)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	//, m_Bones(rhs.m_Bones)					// 뼈 복제 방식 변경
	, m_iNumAnimations(rhs.m_iNumAnimations)	// 애니메이션 개수
	//, m_Animations(rhs.m_Animations)			// 애니메이션 복제 방식 변경
{// !!!! 레퍼런스 카운트 증가가있으면 반드시 감소도 있다 !!!!

	for (auto& pPrototypeAnimation : rhs.m_Animations)	// 애니메이션 벡터 컨테이너에 복사 생성해주는 방식으로 변경
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones) // 뼈 벡터 컨테이너에 복사 생성해주는 방식으로 변경
		m_Bones.push_back(pPrototypeBone->Clone());


	for (auto& MaterialDesc : m_Materials) // 주소를 복사했으니 레퍼런스 카운트를 올려주자
	{
		for (auto& pTexture : MaterialDesc.pMtrlTextures) // 주소를 복사했으니 레퍼런스 카운트를 올려주자
			Safe_AddRef(pTexture);
	}

	for (auto& pMesh : m_Meshes) // 메쉬를 복사하는 만큼 레퍼런스 카운트를 증가시켜준다. [주소참조이기 때문에] (증가했으면 당연히 감소도 신경써야한다.)
	{
		Safe_AddRef(pMesh);
	}
}

HRESULT CModel::Initialize_Prototype(const string& strModelFilePath, _fmatrix PivotMatrix, TYPE eType) // 뼈 세팅 정보 넘겨받기
{
	/*aiProcess_PreTransformVertices | aiProcess_GlobalScale*/	// aiProcess_PreTransformVertices : 값은 제대로 전달됐는데 앞을 안봤던 이유, 이 녀석을 옵션으로 안줘서 모델을 로드할때 행렬 적용을 못시켰음, 원래는 애니메이션이 있는 모델은 이걸로 로드하면 안된다.

	m_eModelType = eType; // 타입 받기

	// 모델을 읽었으니 메쉬를 준비하자.

	_uint	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;


	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1))) // 뼈 먼저 준비
		return E_FAIL;

	if (FAILED(Ready_Meshes(PivotMatrix)))	// 넘겨받은 뼈 세팅 정보를 메쉬 준비 함수에 넘겨주기
		return E_FAIL;

	if (FAILED(Ready_Materials(strModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

// 이 녀석을 호출해서 모델을 그려보자.
HRESULT CModel::Render(_uint iMeshIndex)
{// 메쉬를 전부 순회하며 그린다.
	//for (auto& pMesh : m_Meshes)
	//{
	//	if (nullptr != pMesh)
	//	{
	//		pMesh->Bind_VIBuffers();	// 메쉬의 정점 버퍼를 세팅
	//		pMesh->Render();			// 세팅된 메쉬를 그린다.
	//	}
	//}

	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_Meshes[iMeshIndex]->Bind_VIBuffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

// !!!애니메이션 재생 함수!!!
void CModel::Play_Animation(_float fTimeDelta, _bool isLoop)
{
	// 현재 애니메이션이 애니메이션 최대 개수보다 클경우 재생하지말고 나가자.
	if (m_iCurrentAnimIndex >= m_iNumAnimations) // fiona 기준 NumAnimation 25개
		return;

	/* 현재 애니메이션이 사용하고 있는 뼈들의 TransformationMatrix를 갱신한다. */
	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, fTimeDelta, m_Bones);
	// @@@@@@@@@@@@@@ m_Animations는 애니메이션을 다 가지고 있는 녀석이다. @@@@@@@@@@@@@@@@@@@@@
	// m_iCurrentAnimIndex의 값을 바꿔서 애니메이션을 변경할 수 있다. (외부에서도 할 수 있다는 얘기)
	// ex : 0번째 애니메이션 Idle, 1번째 Attack, 2번째 Jump 등등

	/* 화면에 최종적인 상태로 그려내기위해서는 반드시 뼈들의 CombinedTransformationMatrix가 갱신되어야한다. */
	/* 모든 뼈들을 다 갱신하며 부모로부터 자식까지 쭈우우욱돌아서 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
	}
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones); // 메쉬 클래스에 함수를 호출한다.
}

HRESULT CModel::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pMtrlTextures[eTextureType]->Bind_ShaderResource(pShader, pConstantName);
}

// 메쉬 준비
HRESULT CModel::Ready_Meshes(_fmatrix PivotMatrix) // 뼈 세팅 정보 추가 (전달해줘야 해서)
{
	// m_pAIScene에 담겨있는 메쉬 개수 정보를 m_iNumMeshes에 담아준다.
	m_iNumMeshes = m_pAIScene->mNumMeshes;	// 메쉬 개수 (fiona 5)

	m_Meshes.reserve(m_iNumMeshes);	// 메쉬 개수를 알고있으니, 미리 메모리를 할당해주면 좋을 것이다.

	// 메쉬 개수만큼 순회하자.
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		// 메쉬 개수만큼 메쉬를 생성
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], PivotMatrix, m_Bones); // 생성할때 뼈 정보까지 전달, + 뼈 전달

		if (nullptr == pMesh)
			return E_FAIL; // 실패 (여기서 실패하면 거의 못쓰는 모델..)

		m_Meshes.push_back(pMesh);	// 생성된 메쉬를 벡터에 하나씩 추가.
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const string& strModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MATERIAL_DESC			MaterialDesc = {  };

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			/*for (size_t k = 0; k < pAIMaterial->GetTextureCount(aiTextureType(j)); k++)
			{
				pAIMaterial->GetTexture(aiTextureType(j), k, );
			};*/

			_char		szDrive[MAX_PATH] = "";
			_char		szDirectory[MAX_PATH] = "";

			_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			aiString			strPath;
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			_char		szFileName[MAX_PATH] = "";
			_char		szEXT[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			_char		szTmp[MAX_PATH] = "";
			strcpy_s(szTmp, szDrive);
			strcat_s(szTmp, szDirectory);
			strcat_s(szTmp, szFileName);
			strcat_s(szTmp, szEXT);

			_tchar		szFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szFullPath, MAX_PATH);


			MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath, 1);
			if (nullptr == MaterialDesc.pMtrlTextures[j])
				return E_FAIL;
		}

		m_Materials.push_back(MaterialDesc); // 구조체 null?
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone); // size 1, 2

	_int		iParentIdx = m_Bones.size() - 1; // parentidx 0, 1

	for (size_t i = 0; i < pAINode->mNumChildren; i++) // numchildren 1, 1
	{
		Ready_Bones(pAINode->mChildren[i], iParentIdx);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	// 애니메이션 개수를 받아준다.
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	// 애니메이션 개수만큼 순회한다.
	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		// 애니메이션을 생성해주자.
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones); // 멤버 변수에 저장해둔 뼈 정보를 넘겨주자. 타고 타고 넘어가서 Channel까지 배달될거야
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation); // 생성한 애니메이션 벡터 컨테이너에 담아주기
	}

	return S_OK;
}

// vector
_bool CModel::Compute_MousePos_float4(RAY _Ray, _matrix _WorldMatrix, _float4* pOut)
{
	if (m_Meshes.empty()) // 메쉬가 비었는지 체크
		return false;

	for (auto& iter : m_Meshes)
	{
		if (iter->Compute_MousePos_float4(_Ray, _WorldMatrix, pOut))
			return true;
	}

	return false;
}

// Create 할때 모델 경로를 미리 받는다.
CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix) // 뼈 세팅 정보 받아오기
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath, PivotMatrix, eType))) // 미리 받아온 모델 경로를 넘겨준다. (받은 뼈 세팅 정보 넘겨주기)
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	/* 사본 객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CComponent");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CModel::Free()
{
	__super::Free();


	/* 갖고 있는거 모두 순회해서 삭제(감소) */
	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& MaterialDesc : m_Materials)
	{
		for (auto& pTexture : MaterialDesc.pMtrlTextures)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)	/* 모델의 메쉬와 Importer를 지워주자.*/
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	if (false == m_isCloned) // m_isCloned : 복사 생성은 true, 원본 생성은 false 이다. (이 녀석으로 클론과 원본을 구분하여 삭제한다.)
		m_Importer.FreeScene();
}