#pragma once
#include "Component.h"
//TODO 셀렉터

//! OR 서있거나 앉아있거나 누워있거
 
//TODO 시퀀스
//! AND 탐색 -> 추격 -> 공격


BEGIN(Engine)

class CBehaviorTree final : public CComponent
{
private:
	CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBehaviorTree(const CBehaviorTree& rhs);
	virtual ~CBehaviorTree() = default;

public:
	virtual HRESULT Initialize_Prototype(); //! 프로토타입 패턴을 사용한다는 것을 알 수 있다
	virtual HRESULT Initialize(_uint iNumArg, void* pArgArray);
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

private:
	BT::Builder*	 m_pBuilder = { nullptr };
	shared_ptr<Node> m_pTree;

public:
	static	CBehaviorTree*	Create();
	virtual CComponent*		Clone(_uint iNumArg, void* pArg);
	virtual void			Free() override;
	
};

END

