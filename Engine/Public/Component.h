#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype(); //! 프로토타입 패턴을 사용한다는 것을 알 수 있다
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Tick(_float fTimeDelta) {};
	virtual void	Tick(_float fTimeDelta) {};
	virtual void	Late_Tick(_float fTimeDelta) {};

protected:
	//TODO 컴포넌트가 장치를 들고있어야하는 이유
	//! 공간을 할당하거나(ex. Buffer) 텍스처를 불러온다(ex. Texture) 등을 해야하기 때문에 가지고 있으면 좋으니까

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	_bool					m_isCloned = { false };

public:
	//! 아래의 컴포넌트를 사용하는 이유를 살펴보면 클론을 강제하는 이유를 알 수 있다
	virtual CComponent* Clone(void* pArg) = 0; 
	virtual void Free() override;

};

END

//TODO 컴포넌트를 사용하는 이유

//! 만약, 플레이어로 사용되는 Knight, Archer 클래스가 있고, 몬스터로 사용되는 OrcWarrior, OrcArcher 클래스가 있을 때
//! 플레이어로 사용되는 클래스들은 키입력과 관련된 함수가 필요하고, 몬스터로 사용되는 클래스들은 인공지능을 작성해줘야 하는 함수가 필요하다.
//! 부모클래스를 사용해서 is a 상속관계를 만들어주면 좋으나, Knight 클래스와 OrcWarrior 클래스를 봤을때는 근거리 타입이고, Archer, OrcArcher 클래스는 원거리 타입을 보면
//! 또 한번 분기되는 것을 볼 수 있다.
//! is a 상속관계를 만들어 주기에는 상당히 복잡해 지는 경우가 있다.
//! 소프트웨어 개발에서 사용하는 상속구조를 참고하자니 게임 쪽에서는 객체의 기능이 좀 더 복잡하여 모든 기능을 상속으로 처리하기에는 불편한 점이 많이 보인다.
//! 그래서 착안한 것은 복잡한 상속구조를 사용하지말고 심플한 구조로 가져가는 게 낫지 않을까했다.
//! is a 관계가 아닌 has a 관계를 사용하는 것. ( 파생 시키는 것이 아닌 포함하는 상속구조를 가지자는 것 )
//! Knight 클래스에 경우 키입력과 관련된 함수, 근거리타입의 관련된 함수가 필요했다.
//! Knight 클래스의 멤버변수로 근거리타입 객체를 선언과 할당, 키입력에 대한 객체를 선언과 할당해서 포함시키는 것도 괜찮다는 의견.
//! 즉, 부품과 같이 관련된 기능을 가진 클래스를 포함시켜서 사용하자는 것에서 선택된 디자인 패턴이다. ( 특정 완성제품을 만들기 위한 기능만 따로 뺀 클래스 )
