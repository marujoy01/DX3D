#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	//TODO override의 관한 이야기
	//! 오버라이드를 붙이면 부모클래스의 함수를 호출하지않고, 자식인 내 함수로 호출하라고 선언한 것
	
	virtual HRESULT Initialize_Prototype(const wstring & strHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint		m_iNumVerticesX = { 0 };
	_uint		m_iNumVerticesZ = { 0 };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END