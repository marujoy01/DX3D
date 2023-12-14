#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
private:
	CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring & strNavigationFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

private:
	vector<class CCell*>			m_Cells;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strNavigationFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END