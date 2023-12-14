#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase //TODO 클라이언트단에서 사용해야 하기에 ENGINE_DLL로 Client 프로젝트에게 보여주는 행위 및 부모 클래스로 사용하기에 명시적 선언
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};


END

