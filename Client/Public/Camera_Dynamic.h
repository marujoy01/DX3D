#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

class CCamera_Dynamic final : public CCamera
{
public:
	typedef struct tagDynamicCameraDesc : public CCamera::tagCameraDesc
	{
		_float fMouseSensitivity = 0.0f;
	}DYNAMIC_CAMERA_DESC;

private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

private:
	void			Mouse_Move(_float fTimeDelta);
	void			Mouse_Fix();

private:
	_float			m_fMouseSensitivity = { 0.0f };
	LEVEL			m_eCurrentLevelID = { LEVEL_END };
	_bool			m_bMouseFix = false;

public:
	static CCamera_Dynamic*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};

END

