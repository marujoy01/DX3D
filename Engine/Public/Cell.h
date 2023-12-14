#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase // 베이스 상속
{
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	HRESULT Initialize(const _float3* pPoints);

// 네비메쉬는 Dbug용으로만 랜더하기 때문에 Render를 Debug용으로 감싸준다.
#ifdef _DEBUG
public:
	HRESULT Render(class CShader* pShader);
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_float3					m_vPoints[3] = {};

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual void Free() override;
};

END