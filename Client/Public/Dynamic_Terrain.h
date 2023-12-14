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
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END }; // ����, ����ũ

private:
	CDynamic_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamic_Terrain(const CDynamic_Terrain& rhs);
	virtual ~CDynamic_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel); //! ������ü�� �ʱ�ȭ�� ���� �Լ�.
	virtual HRESULT Initialize(void* pArg) override; //! �纻��ü�� �ʱ�ȭ�� ���� �Լ�. ( void*�� �Ű����ڷ� �޾� Ư���� �纻��ü ó���� �����ϴ�. )
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void	Delete_Component(const wstring& strComTag);
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TYPE_END] = { nullptr }; // ������ ����ũ�� �Ѱ��� ������� �迭�� ���� 0: ���� 2��, 1: ����ũ 1��
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
	static CDynamic_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel); //! ������ü ����
	virtual CGameObject* Clone(void* pArg) override; //! �纻��ü ����
	virtual void			Free() override;

private:
	_float                              m_fBufferDrawRadious = 1.f;
	_float                              m_fBufferPower = 2.f;
	_float4								pOut;

	_vector								m_vMousePos = { 0.f, 0.f, 0.f, 0.f };

	EDIT_MODE                           m_eEditMode = EDIT_MODE::NON;
};

END

