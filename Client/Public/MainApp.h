#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;	//! 게임인스턴스 클래스는 Engine 프로젝트에 있는 클래스. 클라이언트에서 멤버변수로 게임인스턴스의 주소값을 가져오기 위한 전방선언.
END

BEGIN(Client)

class CMainApp final  : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta); //! 기존 프레임워크에서 사용하던 Update 함수의 개념이다. 정쌤 프레임워크에서는 Tick으로 명명해서 사용.
	HRESULT Render();

private:
	//! Direct9 버전에서 사용했던 IDirect3DDevice9 == LPDIRECT3DDEVICE9 컴객체의 기능을 Direct11버전에서는 ID3D11Device, ID3D11DeviceContext 두개로 쪼갰다.
	
	//! ID3D11Device : 사용할 만큼 공간을 할당 해준다  (ex. 텍스처프롬파일, 크리에이트버텍스버퍼 )
	//! 즉, 앞으로 모든 공간을 할당한다는 행위는 ID3D11Device 객체가 수행한다.
	
	
	//! ID3D11DeviceContext : ID3D11Device 객체가 수행하는 공간 할당을 제외한 기타 기능적인 부분들을 수행한다.
	//! 장치를 이용해서 뭔가를 그릴려고 할 때, 어떤 것을 그릴려고 하는지를 장치에 바인딩하는 역할 수행
	
	//! 컴객체를 두개로 나눠서 사용하는 이유 추측 : 어떠한 객체에 ID3D11Device만 던져졌다면 이 객체는 할당을 해주는구나, ID3D11DeviceContext만 던져졌다면 이 객체를 바인딩하고 그리려고 하는구나.라고 추측.
	
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	CGameInstance*					m_pGameInstance = { nullptr };

private:
	HRESULT Open_Level(LEVEL eStartLevelID);
	HRESULT Ready_Prototype_Component_ForStaticLevel();

private:
	HRESULT Ready_Gara();

public:
	static CMainApp* Create(); //! Create 함수는 보통 자식 객체에서 소유하고 있는 함수이다. 최상위 부모함수(Abstract)는 직접 객체화 될 수 없기에 가지고 있지 않다.
	virtual void Free();
};

END