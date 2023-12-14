#pragma once
#include "Base.h"
#include "PipeLine.h"

class CPicker final : public CBase
{
	DECLARE_SINGLETON(CPicker);	// �̱���

public:
	enum class PICKER { LOCAL, WORLD, MAX };

private:
	 CPicker();
	virtual ~CPicker();

public:
	HRESULT Initialize(HWND _hWnd);	// ��ŷ�� ���� ��ġ�� �ڵ��� �޾ƿ´�.
	void	Tick();
	void	Render();

public:
	_bool						Intersect(_Out_ _float3&, const _float3 vA, const _float3 vB, const _float3 vC, _In_opt_ const _float4x4 mWorld);// = _float4x4(g_mUnit)); // g_mUnit = XMMatrixIdentity() : �⺻������ ���� ��� ���� �Ǵ�[����Ʈ�� �����̶�� �����ϸ� �ȴ�]

private:
	_float3						m_vRay[IDX(PICKER::MAX)]{};
	_float3						m_vOrg[IDX(PICKER::MAX)]{};

private:
	class CPipeLine* m_pPipeLine = { nullptr };

private:
	HWND			m_hWnd = 0;	// �ڵ��� �ޱ����� �׸�
};

