#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
//class CVIBuffer_Terrain;
class CVIBuffer_Dynamic_Terrain;
END

BEGIN(Client)

/* Dynamic Terrain*/
typedef struct tagVertex_Dynamic_Info
{
	float		fX;
	float		fY;
	float		fZ;
}DINFO;

class CDynamic_Terrain final : public CGameObject
{
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END }; // 지형, 마스크

private:
	CDynamic_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamic_Terrain(const CDynamic_Terrain& rhs);
	virtual ~CDynamic_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel); //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void	Delete_Component(const wstring& strComTag);
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TYPE_END] = { nullptr }; // 지형과 마스크를 한곳에 담기위해 배열로 변경 0: 지형 2장, 1: 마스크 1장
	CVIBuffer_Dynamic_Terrain* m_pVIBufferCom = { nullptr };
	//CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };

private:
	LEVEL				m_eCurrentLevelID = { LEVEL_END };
	//VTXDYNAMIC* m_test;
	DINFO m_tDynamicInfo;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void	ReceiveInfo(DINFO pInfo);

public:
	void	PickingGround(EDIT_MODE _eMode);
	_bool	PickingGround(_float4* pOut);

public:
	static CDynamic_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual void			Free() override;

private:
	_float                              m_fBufferDrawRadious = 1.f;
	_float                              m_fBufferPower = 2.f;
	_float4								pOut;

	_vector								m_vMousePos = { 0.f, 0.f, 0.f, 0.f };

	EDIT_MODE                           m_eEditMode = EDIT_MODE::NON;
};

END

