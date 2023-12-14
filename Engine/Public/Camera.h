#pragma once

#include "GameObject.h"


//TODO 카메라 클래스의 역할
//! 클라이언트 개발자가 만들고자 하는 카메라들의 공통적인 부모가 되기위한 클래스

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public tagGameObjectDesc
	{
		_float4 vEye, vAt;
		_float	fFovY, fAspect, fNear, fFar;
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

protected:
	//TODO 멤버변수로 들고있는 이유, eye와 at은 없는이유
	//! 먼저 멤버변수로 들고있는 이유는 항상 값이 고정이 아니란 이야기. 값에 변경에따라 처리 한다와 직결된다.
	//! eye와 at같은 경우에는 한번만 주고 나중에 Transform 컴포넌트로 제어할 것이기때문에 멤버변수로 두지 않는다.

	//! 투영행렬을 만들기위한 데이터를 영구적으로 보관한 것. 루프를돌면서 투영행렬을 만들것이다. 값이 바뀌면 당연히 바뀌겟지.
	_float			m_fFovy = { 0.0f };
	_float			m_fAspect = { 0.0f };
	_float			m_fNear = { 0.0f };
	_float			m_fFar = { 0.0f };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void		 Free() override;

};

END