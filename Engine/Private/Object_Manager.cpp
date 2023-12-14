#include "Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == pPrototype || nullptr != Find_Prototype(strPrototypeTag)) //todo || 문을 사용할때는 반드시 좀 더 간단한 조건을 앞에 놓아서 효율적으로
			return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pPrototype); //! 원형객체 추가.
	
	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg, CGameObject** ppOut) // 이중포인터로 Out 인자 만들기
{
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag); //! 원형객체 탐색

	if(nullptr == pPrototype)
		return E_FAIL;

	CGameObject*	pGameObject = pPrototype->Clone(pArg); // 사본객체 생성

	if(nullptr == pGameObject)
		return E_FAIL;

	if (nullptr != ppOut)
		*ppOut = pGameObject;	// 만약 만들었다면 만든적 있다고 알려주기 위해 값을 반환해주기 (이 값이 있으면 삭제하고 다시생성, 없으면 그냥 생성하려고.)

	CLayer*			pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer) //! 해당 이름을 가진 레이어가 아직 존재하지않았다. 이 이름을 가진 레이어에 최초로 추가.
	{
		pLayer = CLayer::Create();

		//if (nullptr != ppOut)
		//*ppOut = pGameObject;	// 오브젝트는 삭제했지만, 레이어는 남아있기때문에 이 조건문을 다시 안탄다. -> 따라서 밖에다 선언해주자. (44줄)

		if(nullptr == pLayer)
			return E_FAIL;
	
		pLayer->Add_GameObject(pGameObject); //! 만들어낸 레이어에 새로 생성한 사본객체 넣어줌
		m_pLayers[iLevelIndex].emplace(strLayerTag,pLayer); //! 생성한 레이어 추가해줌.
	}
	else //! 이미 이름을 가진 레이어가 있었다.
		pLayer->Add_GameObject(pGameObject);


	return S_OK;
}

//! Tick을 따로 함수로 따로 나눈이유 : 모든 객체들의 Priority_Tick을 순회 후 Tick 순회 후 Late_Tick을 순회하고 싶었다.
//! 그렇다면 한 함수에서 폴문을 단계별로 돌리면 되지 않았냐 : 다른 셋팅을 해주고 싶었다. 예를들어 Priority_Tick을 수행 후 그래픽디바이스의 셋팅값을 바꾸고 Tick을 하고싶었다 같은 경우에 제어하고싶어서.

void CObject_Manager::Priority_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Priority_Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Late_Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for(auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

CGameObject* CObject_Manager::Find_Prototype(const wstring& strPrototypeTag)
{
	auto& iter = m_Prototypes.find(strPrototypeTag);

	if(iter == m_Prototypes.end())
		return nullptr;

	return iter->second;	
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if(iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto iter = m_pLayers[iLevelIndex].find(strLayerTag);
	
	if(iter == m_pLayers[iLevelIndex].end())
		return nullptr;
	
	return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for(auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	for(auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}

