#pragma once
#include "Base.h"
#include "PipeLine.h"

class CPicker final : public CBase
{
	DECLARE_SINGLETON(CPicker);	// 싱글톤

public:
	enum class PICKER { LOCAL, WORLD, MAX };

private:
	 CPicker();
	virtual ~CPicker();

public:
	HRESULT Initialize(HWND _hWnd);	// 픽킹을 위해 장치에 핸들을 받아온다.
	void	Tick();
	void	Render();

public:
	_bool						Intersect(_Out_ _float3&, const _float3 vA, const _float3 vB, const _float3 vC, _In_opt_ const _float4x4 mWorld);// = _float4x4(g_mUnit)); // g_mUnit = XMMatrixIdentity() : 기본값으로 단위 행렬 여부 판단[디폴트값 지정이라고 생각하면 된다]

private:
	_float3						m_vRay[IDX(PICKER::MAX)]{};
	_float3						m_vOrg[IDX(PICKER::MAX)]{};

private:
	class CPipeLine* m_pPipeLine = { nullptr };

private:
	HWND			m_hWnd = 0;	// 핸들을 받기위한 그릇
};

