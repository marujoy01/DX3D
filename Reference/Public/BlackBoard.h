#pragma once

#include "Component.h"

//TODO 행동트리(Behavior Tree)에서 사용되는 개념이다
//! 키, 밸류 형태로 구성된다.
//! 각 키는 특정 정보나 변수를 나타낸다
//! 위의 키, 밸류로 구성된 페어들은 블랙보드 컴포넌트에 저장되어 게임 객체(NPC,AI 캐릭터)의 현재 상태를 나타낸다.
//! 블랙보드의 내용은 행동트리 노드에서 사용되어 게임 로직을 구동하거나 결정을 내리는 데 활용된다.
//! 예를 들자면, 적 캐릭터가 플레이어를 추적하도록 하는 행동 트리를 구현한다고 가정하자.
//! 행동트리의 노드는 블랙보드에서 이 정보를 읽어와서 플레이어를 추적하는 등의 행동을 수행할 수 있다.

//TODO 그래서 블랙보드가 뭐냐고
//! 인공지능의 판단에 사용하는 데이터들의 집합을 의미한다
//! NPC의 의사 결정은 블랙보드 데이터 기반으로 진행 된다는 말.

//TODO 생각해보자 어떤 인공지능의 판단에 사용되는 데이터들이 뭐가 있을까
//! 추적으로 예를 들자면 상대방(적군)의 위치와 상태값이 필요하다
//! 상태값은 왜? 죽었는지, 살았는지, 무적상태인지, 은신상태인지 알아야 될 수도 있잖아
//! 

//TODO 키밸류는 어떤식으로 줘야해?


BEGIN(Engine)

class CBlackBoard final : public CComponent
{
private:
	CBlackBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlackBoard(const CBlackBoard& rhs);
	virtual ~CBlackBoard() = default;

public:
	HRESULT		Initialize();
	
public:
	static  CBlackBoard* Create();
	virtual CComponent*  Clone(void* pArg) override;
	virtual void Free() override;

private:
	class CBTRoot*			m_pRootTree;
};

END
