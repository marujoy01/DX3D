#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CNavigation; // 전방 선언 추가
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END }; // 지형, 마스크

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel); //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom[TYPE_END] = { nullptr }; // 지형과 마스크를 한곳에 담기위해 배열로 변경 0: 지형 2장, 1: 마스크 1장
	CVIBuffer_Terrain*			m_pVIBufferCom = { nullptr };
	CNavigation*				m_pNavigationCom = { nullptr }; // 네비게이션 추가
	CVIBuffer_Dynamic_Terrain*	m_pVIBufferDynamicCom = { nullptr };

private:
	LEVEL				m_eCurrentLevelID = { LEVEL_END };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel); //! 원형객체 생성
	virtual CGameObject*	Clone(void* pArg) override; //! 사본객체 생성
	virtual void			Free() override;
};

END

