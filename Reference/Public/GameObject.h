#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc
	{
		tagGameObjectDesc() {}
		tagGameObjectDesc(_float fSpeed, _float fRotation)
			: fSpeedPerSec(fSpeed), fRotationPerSec(fRotation) {}

		_float	fSpeedPerSec		= 0.f;
		_float	fRotationPerSec		= 0.f;
		_vector vPosition			= { 0.f, 0.f, 0.f, 0.f }; // 위치 추가.

	}GAMEOBJECT_DESC;



protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Tick(_float fTimeDelta);
	virtual void	Tick(_float fTimeDelta);
	virtual void	Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public: // CBase 상속 함수
	virtual void Write_Json(json& Out_Json);	// 저장
	virtual void Load_FromJson(const json& In_Json); // 불러오기

public:
	class CTransform* Get_TransformCom() { return m_pTransformCom; }

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

protected:
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	class CTransform*			m_pTransformCom = { nullptr };

	map<const wstring, class CComponent*>		m_Components;

protected:
	_bool						m_isCloned = { false };

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag,
			const wstring& strComTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);

	void	Delete_Component(const wstring& strComTag);

	class CComponent* Find_Component(const wstring& strComTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END