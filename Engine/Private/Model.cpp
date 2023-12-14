#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)	// ���� ������. (Ŭ���� ���鶧 ���� ����, Ŭ�� ��������� ����������.)
	: CComponent(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_eModelType(rhs.m_eModelType)
	, m_iNumMeshes(rhs.m_iNumMeshes)	// �޽� ���� ������� ����
	, m_Meshes(rhs.m_Meshes)			// �޽����� ������� ���� (vector)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	//, m_Bones(rhs.m_Bones)					// �� ���� ��� ����
	, m_iNumAnimations(rhs.m_iNumAnimations)	// �ִϸ��̼� ����
	//, m_Animations(rhs.m_Animations)			// �ִϸ��̼� ���� ��� ����
{// !!!! ���۷��� ī��Ʈ ������������ �ݵ�� ���ҵ� �ִ� !!!!

	for (auto& pPrototypeAnimation : rhs.m_Animations)	// �ִϸ��̼� ���� �����̳ʿ� ���� �������ִ� ������� ����
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones) // �� ���� �����̳ʿ� ���� �������ִ� ������� ����
		m_Bones.push_back(pPrototypeBone->Clone());


	for (auto& MaterialDesc : m_Materials) // �ּҸ� ���������� ���۷��� ī��Ʈ�� �÷�����
	{
		for (auto& pTexture : MaterialDesc.pMtrlTextures) // �ּҸ� ���������� ���۷��� ī��Ʈ�� �÷�����
			Safe_AddRef(pTexture);
	}

	for (auto& pMesh : m_Meshes) // �޽��� �����ϴ� ��ŭ ���۷��� ī��Ʈ�� ���������ش�. [�ּ������̱� ������] (���������� �翬�� ���ҵ� �Ű����Ѵ�.)
	{
		Safe_AddRef(pMesh);
	}
}

HRESULT CModel::Initialize_Prototype(const string& strModelFilePath, _fmatrix PivotMatrix, TYPE eType) // �� ���� ���� �Ѱܹޱ�
{
	/*aiProcess_PreTransformVertices | aiProcess_GlobalScale*/	// aiProcess_PreTransformVertices : ���� ����� ���޵ƴµ� ���� �Ⱥô� ����, �� �༮�� �ɼ����� ���༭ ���� �ε��Ҷ� ��� ������ ��������, ������ �ִϸ��̼��� �ִ� ���� �̰ɷ� �ε��ϸ� �ȵȴ�.

	m_eModelType = eType; // Ÿ�� �ޱ�

	// ���� �о����� �޽��� �غ�����.

	_uint	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;


	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1))) // �� ���� �غ�
		return E_FAIL;

	if (FAILED(Ready_Meshes(PivotMatrix)))	// �Ѱܹ��� �� ���� ������ �޽� �غ� �Լ��� �Ѱ��ֱ�
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

// �� �༮�� ȣ���ؼ� ���� �׷�����.
HRESULT CModel::Render(_uint iMeshIndex)
{// �޽��� ���� ��ȸ�ϸ� �׸���.
	//for (auto& pMesh : m_Meshes)
	//{
	//	if (nullptr != pMesh)
	//	{
	//		pMesh->Bind_VIBuffers();	// �޽��� ���� ���۸� ����
	//		pMesh->Render();			// ���õ� �޽��� �׸���.
	//	}
	//}

	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_Meshes[iMeshIndex]->Bind_VIBuffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

// !!!�ִϸ��̼� ��� �Լ�!!!
void CModel::Play_Animation(_float fTimeDelta, _bool isLoop)
{
	// ���� �ִϸ��̼��� �ִϸ��̼� �ִ� �������� Ŭ��� ����������� ������.
	if (m_iCurrentAnimIndex >= m_iNumAnimations) // fiona ���� NumAnimation 25��
		return;

	/* ���� �ִϸ��̼��� ����ϰ� �ִ� ������ TransformationMatrix�� �����Ѵ�. */
	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, fTimeDelta, m_Bones);
	// @@@@@@@@@@@@@@ m_Animations�� �ִϸ��̼��� �� ������ �ִ� �༮�̴�. @@@@@@@@@@@@@@@@@@@@@
	// m_iCurrentAnimIndex�� ���� �ٲ㼭 �ִϸ��̼��� ������ �� �ִ�. (�ܺο����� �� �� �ִٴ� ���)
	// ex : 0��° �ִϸ��̼� Idle, 1��° Attack, 2��° Jump ���

	/* ȭ�鿡 �������� ���·� �׷��������ؼ��� �ݵ�� ������ CombinedTransformationMatrix�� ���ŵǾ���Ѵ�. */
	/* ��� ������ �� �����ϸ� �θ�κ��� �ڽı��� �޿����Ƽ� CombinedTransformationMatrix�� �����Ѵ�. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
	}
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones); // �޽� Ŭ������ �Լ��� ȣ���Ѵ�.
}

HRESULT CModel::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pMtrlTextures[eTextureType]->Bind_ShaderResource(pShader, pConstantName);
}

// �޽� �غ�
HRESULT CModel::Ready_Meshes(_fmatrix PivotMatrix) // �� ���� ���� �߰� (��������� �ؼ�)
{
	// m_pAIScene�� ����ִ� �޽� ���� ������ m_iNumMeshes�� ����ش�.
	m_iNumMeshes = m_pAIScene->mNumMeshes;	// �޽� ���� (fiona 5)

	m_Meshes.reserve(m_iNumMeshes);	// �޽� ������ �˰�������, �̸� �޸𸮸� �Ҵ����ָ� ���� ���̴�.

	// �޽� ������ŭ ��ȸ����.
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		// �޽� ������ŭ �޽��� ����
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], PivotMatrix, m_Bones); // �����Ҷ� �� �������� ����, + �� ����

		if (nullptr == pMesh)
			return E_FAIL; // ���� (���⼭ �����ϸ� ���� ������ ��..)

		m_Meshes.push_back(pMesh);	// ������ �޽��� ���Ϳ� �ϳ��� �߰�.
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

		m_Materials.push_back(MaterialDesc); // ����ü null?
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
	// �ִϸ��̼� ������ �޾��ش�.
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	// �ִϸ��̼� ������ŭ ��ȸ�Ѵ�.
	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		// �ִϸ��̼��� ����������.
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones); // ��� ������ �����ص� �� ������ �Ѱ�����. Ÿ�� Ÿ�� �Ѿ�� Channel���� ��޵ɰž�
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation); // ������ �ִϸ��̼� ���� �����̳ʿ� ����ֱ�
	}

	return S_OK;
}

// vector
_bool CModel::Compute_MousePos_float4(RAY _Ray, _matrix _WorldMatrix, _float4* pOut)
{
	if (m_Meshes.empty()) // �޽��� ������� üũ
		return false;

	for (auto& iter : m_Meshes)
	{
		if (iter->Compute_MousePos_float4(_Ray, _WorldMatrix, pOut))
			return true;
	}

	return false;
}

// Create �Ҷ� �� ��θ� �̸� �޴´�.
CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix) // �� ���� ���� �޾ƿ���
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath, PivotMatrix, eType))) // �̸� �޾ƿ� �� ��θ� �Ѱ��ش�. (���� �� ���� ���� �Ѱ��ֱ�)
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	/* �纻 ��ü�� �ʱ�ȭ�Ѵ�.  */
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


	/* ���� �ִ°� ��� ��ȸ�ؼ� ����(����) */
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

	for (auto& pMesh : m_Meshes)	/* ���� �޽��� Importer�� ��������.*/
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	if (false == m_isCloned) // m_isCloned : ���� ������ true, ���� ������ false �̴�. (�� �༮���� Ŭ�а� ������ �����Ͽ� �����Ѵ�.)
		m_Importer.FreeScene();
}