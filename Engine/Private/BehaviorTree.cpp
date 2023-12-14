#include "BehaviorTree.h"

CBehaviorTree::CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
: CComponent(pDevice,pContext)
{
}

CBehaviorTree::CBehaviorTree(const CBehaviorTree& rhs)
: CComponent(rhs)
{

}

HRESULT CBehaviorTree::Initialize_Prototype()
{
	m_pBuilder = &BT::Builder();
	m_pTree = m_pBuilder->build();
	
	return S_OK;
}

HRESULT CBehaviorTree::Initialize(_uint iNumArg, void* pArg)
{
	string test;


	test = typeid(pArg).name();

	

	return S_OK;
}

void CBehaviorTree::Priority_Tick(_float fTimeDelta)
{
}

void CBehaviorTree::Tick(_float fTimeDelta)
{
}

void CBehaviorTree::Late_Tick(_float fTimeDelta)
{
}

CBehaviorTree* CBehaviorTree::Create()
{
	return nullptr;
}

CComponent* CBehaviorTree::Clone(_uint iNumArg, void* pArg)
{
	return nullptr;
}

void CBehaviorTree::Free()
{
	
}
